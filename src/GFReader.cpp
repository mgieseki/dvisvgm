/***********************************************************************
** GFReader.cpp                                                       **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/

#include <cstdarg>
#include <iostream>
#include "GFReader.h"
#include "Message.h"
#include "macros.h"

using namespace std;

struct GFCommand 
{
	void (GFReader::*method)(int);
	int numBytes;
};

struct CharInfo
{
	CharInfo (Int32 dxx, Int32 dyy, Int32 w, UInt32 p) 
		: dx(dxx), dy(dyy), width(w), location(p) {}
	
	Int32 dx, dy;
	Int32 width;
	UInt32 location;
};


static inline double fix2double (Int32 fix) {
	return double(fix)/(1 << 20);
}


static inline double scaled2double (Int32 scaled) {
	return double(scaled)/(1 << 16);
}


static void emitError (const GFReader &reader, const char *msg, ...) {
	va_list va;
	va_start(va, msg);
	char buf[256];
	vsprintf(buf, msg, va);
	va_end(va);
	reader.error(buf);
}


GFReader::GFReader (istream &is) : in(is), valid(true)
{
	minX = maxX = minY = maxY = x = y = 0;
}


GFReader::~GFReader () {
	FORALL(charInfoMap, Iterator, i)
		delete i->second;
}


UInt32 GFReader::readUnsigned (int bytes) {
	UInt32 ret = 0;
	for (int i=bytes-1; i >= 0 && !in.eof(); i--) {
		UInt32 b = in.get();
		ret |= b << (8*i);
	}
	return ret;
}


Int32 GFReader::readSigned (int bytes) {
	Int32 ret = in.get();
	if (ret & 128)        // negative value?
		ret |= 0xffffff00;
	for (int i=bytes-2; i >= 0 && !in.eof(); i--) 
		ret = (ret << 8) | in.get();
	return ret;
}


string GFReader::readString (int bytes) {
	char *buf = new char[bytes+1];
	if (bytes > 0)
		in.get(buf, bytes+1);  // reads 'bytes' bytes (pos. bytes+1 is set to 0)
	else
		*buf = 0;
	string ret = buf;
	delete [] buf;
	return ret;
}


void GFReader::error (string msg) const {
	Message::estream(true) << msg << endl;
}


/** Reads a single GF command from the current position of the input stream and calls the
 *  corresponding cmdFOO method.  
 *  @return opcode of the executed command */
int GFReader::executeCommand () {
   /* Each cmdFOO command reads the necessary number of bytes from the stream so executeCommand
   doesn't need to know the exact GF command format. Some cmdFOO methods are used for multiple
	GF commands because they only differ in the size of their parameters. */
	static const GFCommand commands[] = {
		{&GFReader::cmdPaint, 1}, {&GFReader::cmdPaint, 2}, {&GFReader::cmdPaint, 3},                        // 64-66
		{&GFReader::cmdBoc, 0},   {&GFReader::cmdBoc1, 0},                                                   // 67-68
		{&GFReader::cmdEoc, 0},                                                                              // 69
		{&GFReader::cmdSkip, 0},  {&GFReader::cmdSkip, 1},  {&GFReader::cmdSkip, 2},{&GFReader::cmdSkip, 3}, // 70-73
		{&GFReader::cmdXXX, 1},   {&GFReader::cmdXXX, 2},   {&GFReader::cmdXXX, 3}, {&GFReader::cmdXXX, 4},  // 239-242
		{&GFReader::cmdYYY, 0},                                                                              // 243
		{&GFReader::cmdNop, 0},                                                                              // 244
		{&GFReader::cmdCharLoc, 0}, {&GFReader::cmdCharLoc0, 0},                                             // 245-246
		{&GFReader::cmdPre, 0},     {&GFReader::cmdPost, 0}, {&GFReader::cmdPostPost, 0}                     // 247-249
	};
	
	int opcode = in.get();
	if (opcode < 0) { // at end of file
		error("unexpected end of file");
		return 249;   // postpost opcode => stop further reading
	}
	if (opcode >= 0 && opcode <= 63)
		cmdPaint0(opcode);
	else if (opcode >= 74 && opcode <= 238)
		cmdNewRow(opcode-74);
	else if (opcode >= 250) {
		emitError(*this, "undefined GF command (opcode %d)", opcode);
		valid = false;
	}
	else {
		int offset = opcode <= 73 ? 64 : 239-(73-64+1);
		const GFCommand &cmd = commands[opcode-offset];
		if (cmd.method)
			(this->*cmd.method)(cmd.numBytes);
	}
	return opcode;
}


bool GFReader::executeChar (UInt8 c) {
	in.clear();
	if (charInfoMap.empty())
		executePostamble();          // read character infos
	in.clear();
	Iterator it = charInfoMap.find(c);
	if (in && valid && it != charInfoMap.end()) {
		in.seekg(it->second->location, ios_base::beg);
		while (valid && executeCommand() != 69);  // execute all commands until eoc is reached
		return valid;
	}
	return false;
}


/*
bool GFReader::executeAllChars () {
	in.clear();
	if (charInfoMap.empty())
		executePostamble();          // read character infos
	if (in && valid)
		FORALL(charInfoMap, Iterator, i) {
			in.seekg(i->second->location, ios_base::beg);
			while (valid && executeCommand() != 69);  // execute all commands until eoc is reached
		}	
}*/


bool GFReader::executePostamble () {
	in.clear();
	if (!in)
		return false;
	in.seekg(-1, ios_base::end);
	while (in.peek() == 223)         // skip fill bytes
		in.seekg(-1, ios_base::cur);
	in.seekg(-4, ios_base::cur);
	UInt32 q = readUnsigned(4);      // pointer to begin of postamble
	in.seekg(q, ios_base::beg);    // now on begin of postamble
	while (valid && executeCommand() != 249); // execute all commands until postpost is reached
	return valid;
}


/** Returns the design size of this font int TeX point units. */
double GFReader::getDesignSize () const {
	return fix2double(designSize);
}

/** Returns the number of horizontal pixels per point. */
double GFReader::getHPixelsPerPoint () const {
	return scaled2double(hppp);
}

/** Returns the number of vertical pixels per point. */
double GFReader::getVPixelsPerPoint () const {
	return scaled2double(vppp);
}

/** Returns the width of character c in TeX point units */
double GFReader::getCharWidth (UInt32 c) const {
	ConstIterator it = charInfoMap.find(c%256);
	return it == charInfoMap.end() ? 0 : it->second->width*getDesignSize()/(1<<24);
}

///////////////////////


void GFReader::cmdPre (int) {
	UInt32 i = readUnsigned(1);
	if (i == 131) {
		UInt32 k = readUnsigned(1);
		string s = readString(k);
		preamble(s);
	}
	else {
		error ("invalid identification number in GF preamble");
		valid = false;
	}
}


void GFReader::cmdPost (int) {
	readUnsigned(4);               // pointer to byte after final eoc
	designSize = readUnsigned(4);  // design size of font in points
	checksum   = readUnsigned(4);  // checksum
	hppp       = readUnsigned(4);  // horizontal pixels per point (scaled int)
	vppp       = readUnsigned(4);  // vertical pixels per point (scaled int)
	in.seekg(16, ios_base::cur);   // skip x and y bounds
	postamble();
}


void GFReader::cmdPostPost (int) {
	readUnsigned(4);   // pointer to begin of postamble
	UInt32 i = readUnsigned(1);
	if (i == 131) 
		while (readUnsigned(1) == 223); // skip fill bytes
	else {
		error ("invalid identification number in GF preamble");
		valid = false;
	}
}


void GFReader::cmdPaint0 (int pixels) {
	if (penDown)                      // set pixels?
		bitmap.setBits(y, x, pixels);
	x += pixels;
	penDown = !penDown;               // inverse pen state
}


void GFReader::cmdPaint (int len) {
	UInt32 pixels = readUnsigned(len);
	cmdPaint0(pixels);
}


void GFReader::cmdBoc (int) {
	currentChar = readSigned(4);
	readSigned(4);  // pointer to previous boc with same c mod 256
	minX = readSigned(4);
	maxX = readSigned(4);
	minY = readSigned(4);
	maxY = readSigned(4);
	x = minX;
	y = maxY;
	penDown = false;
	bitmap.resize(minX, maxX, minY, maxY);
	beginChar(currentChar);
}


void GFReader::cmdBoc1 (int) {
	currentChar = readUnsigned(1);
	UInt32 dx = readUnsigned(1);
	maxX = readUnsigned(1);
	minX = maxX - dx;
	UInt32 dy = readUnsigned(1);
	maxY = readUnsigned(1);
	minY = maxY - dy;
	x = minX;
	y = maxY;
	penDown = false;
	bitmap.resize(minX, maxX, minY, maxY);
	beginChar(currentChar);
}


void GFReader::cmdEoc (int) {
	endChar(currentChar);
}


void GFReader::cmdSkip (int len) {
	if (len == 0)
		y--;
	else 
		y -= readUnsigned(len)+1;
	x = minX;
	penDown = false;
}


void GFReader::cmdNewRow (int col) {
	x = minX + col ;
	y--;
	penDown = true;
}


void GFReader::cmdXXX (int len) {
	UInt32 n = readUnsigned(len);
	string str = readString(n);
	special(str);
}


void GFReader::cmdYYY (int) {
	Int32 y = readSigned(4);
	numspecial(y);
}


void GFReader::cmdNop (int) {
}


void GFReader::cmdCharLoc0 (int) {	
	UInt8 c  = readUnsigned(1); // character code mod 256
	UInt8 dm = readUnsigned(1); // 
	Int32 w  = readSigned(4);   // (1<<24)*characterWidth/designSize
	Int32 p   = readSigned(4);  // pointer to begin of (last) character data
	Int32 dx  = 65536*dm;
	Int32 dy  = 0;
	charInfoMap[c] = new CharInfo(dx, dy, w, p);
}


void GFReader::cmdCharLoc (int) {
	UInt32 c = readUnsigned(1); // character code mod 256
	Int32 dx = readSigned(4);   // horizontal escapement (scaled pixel units)
	Int32 dy = readSigned(4);   // vertical escapement (scaled pixel units)
	Int32 w  = readSigned(4);   // (1<<24)*characterWidth/designSize
	Int32 p  = readSigned(4);   // pointer to begin of (last) character data
	charInfoMap[c] = new CharInfo(dx, dy, w, p);
}

