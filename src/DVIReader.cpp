/***********************************************************************
** DVIReader.cpp                                                      **
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
#include <fstream>
#include <iostream>
#include <sstream>
#include "types.h"
#include "DVIActions.h"
#include "DVIReader.h"
#include "Font.h"
#include "FontManager.h"
#include "Message.h"
#include "VectorStream.h"
#include "macros.h"

using namespace std;


DVIReader::DVIReader (istream &is, DVIActions *a) 
	: StreamReader(is), actions(a) 
{
	inPage = false;
	pageHeight = pageWidth = 0;
	scaleFactor = 0.0;
	inPostamble = false;
	fontManager = new FontManager;
}


DVIReader::~DVIReader () {
	delete fontManager;
}



DVIActions* DVIReader::replaceActions (DVIActions *a) {
	DVIActions *prev_actions = actions;
	actions = a;
	return prev_actions;
}


/** Reads a single DVI command from the current position of the input stream and calls the
 *  corresponding cmdFOO method.  
 *  @return opcode of the executed command */
int DVIReader::executeCommand () {
	struct DVICommand {
		void (DVIReader::*method)(int);
		int numBytes;
	};

   /* Each cmdFOO command reads the necessary number of bytes from the stream, so executeCommand
   doesn't need to know the exact DVI command format. Some cmdFOO methods are used for multiple
	DVI commands because they only differ in length of their parameters. */
	static const DVICommand commands[] = {
		{&DVIReader::cmdSetChar, 1}, {&DVIReader::cmdSetChar, 2}, {&DVIReader::cmdSetChar, 3}, {&DVIReader::cmdSetChar, 4}, // 128-131
		{&DVIReader::cmdSetRule, 0},                                                                                        // 132
		{&DVIReader::cmdPutChar, 1}, {&DVIReader::cmdPutChar, 2}, {&DVIReader::cmdPutChar, 3}, {&DVIReader::cmdPutChar, 4}, // 133-136
		{&DVIReader::cmdPutRule, 0},                                                                                        // 137
		{&DVIReader::cmdNop, 0},                                                                                            // 138
		{&DVIReader::cmdBop, 0},     {&DVIReader::cmdEop, 0},                                                               // 139-140
		{&DVIReader::cmdPush, 0},    {&DVIReader::cmdPop, 0},                                                               // 141-142
		{&DVIReader::cmdRight, 1},   {&DVIReader::cmdRight, 2},   {&DVIReader::cmdRight, 3},   {&DVIReader::cmdRight, 4},   // 143-146
		{&DVIReader::cmdW0, 0},                                                                                             // 147
		{&DVIReader::cmdW, 1},       {&DVIReader::cmdW, 2},       {&DVIReader::cmdW, 3},       {&DVIReader::cmdW, 4},       // 148-151
		{&DVIReader::cmdX0, 0},                                                                                             // 152
		{&DVIReader::cmdX, 1},       {&DVIReader::cmdX, 2},       {&DVIReader::cmdX, 3},       {&DVIReader::cmdX, 4},       // 153-156
		{&DVIReader::cmdDown, 1},    {&DVIReader::cmdDown, 2},    {&DVIReader::cmdDown, 3},    {&DVIReader::cmdDown, 4},    // 157-160
		{&DVIReader::cmdY0, 0},                                                                                             // 161
		{&DVIReader::cmdY, 1},       {&DVIReader::cmdY, 2},       {&DVIReader::cmdY, 3},       {&DVIReader::cmdY, 4},       // 162-165
		{&DVIReader::cmdZ0, 0},                                                                                             // 166
		{&DVIReader::cmdZ, 1},       {&DVIReader::cmdZ, 2},       {&DVIReader::cmdZ, 3},       {&DVIReader::cmdZ, 4},       // 167-170
		{&DVIReader::cmdFontNum, 1}, {&DVIReader::cmdFontNum, 2}, {&DVIReader::cmdFontNum, 3}, {&DVIReader::cmdFontNum, 4}, // 235-238
		{&DVIReader::cmdXXX, 1},     {&DVIReader::cmdXXX, 2},     {&DVIReader::cmdXXX, 3},     {&DVIReader::cmdXXX, 4},     // 239-242
		{&DVIReader::cmdFontDef, 1}, {&DVIReader::cmdFontDef, 2}, {&DVIReader::cmdFontDef, 3}, {&DVIReader::cmdFontDef, 4}, // 243-246
		{&DVIReader::cmdPre, 0},     {&DVIReader::cmdPost, 0},    {&DVIReader::cmdPostPost, 0}                              // 247-249
	};
	
	int opcode = in().get();
	if (!in() || opcode < 0)  // at end of file
		throw InvalidDVIFileException("invalid file");
	if (opcode >= 0 && opcode <= 127)
		cmdSetChar0(opcode);
	else if (opcode >= 171 && opcode <= 234)
		cmdFontNum0(opcode-171);
	else if (opcode >= 250) {
		ostringstream oss; 
		oss << "undefined DVI command (opcode " << opcode << ')';
		throw DVIException(oss.str());
	}
	else {
		int offset = opcode <= 170 ? 128 : 235-(170-128+1);
		const DVICommand &cmd = commands[opcode-offset];
		(this->*cmd.method)(cmd.numBytes);
	}
	return opcode;
}


/** Executes all DVI commands read from the input stream. */
void DVIReader::executeAll () {
	int opcode = 0;
	while (!in().eof() && opcode >= 0) {
		try {
			opcode = executeCommand();
		}
		catch (const InvalidDVIFileException &e) {
			// end of stream reached
			opcode = -1;
		}
	}
}


/** Executes all DVI commands from the preamble to postpost. */
bool DVIReader::executeDocument () {
	in().clear();   // reset all status bits
	if (!in())
		return false;
	in().seekg(0);  // move file pointer to first byte of the input stream
	while (!in().eof() && executeCommand() != 249); // stop reading after postpost (249)
	return true;
}


bool DVIReader::executeAllPages () {
	in().clear();   // reset all status bits
	if (!in())
		return false;
	in().seekg(0);  // move file pointer to first byte of the input stream
	while (!in().eof() && executeCommand() != 248); // stop reading when postamble (248) is reached
	return true;
}


/** Reads and executes the commands of a single page. 
 *  This methods stops reading after the page's eop command has been executed. 
 *  @param[in] n number of page to be executed
 *  @returns true if page was read successfully */
bool DVIReader::executePage (unsigned n) {
	in().clear();    // reset all status bits
	if (!in())
		throw DVIException("invalid DVI file");
	in().seekg(-1, ios_base::end);      // stream pointer to last byte
	while (in().peek() == 223) 
		in().seekg(-1, ios_base::cur);   // skip fill bytes
	in().seekg(-4, ios_base::cur);      // now on first byte of q (pointer to begin of postamble)
	UInt32 q = readUnsigned(4);         // pointer to begin of postamble
	in().seekg(q, ios_base::beg);       // now on begin of postamble
	if (executeCommand() != 248)        // execute postamble command but not the fontdefs
		return false;
	if (n < 1 || n > totalPages) 
		return false;
	in().seekg(prevBop, ios_base::beg); // now on last bop
	inPostamble = false;                // we jumped out of the postamble
	unsigned pageCount = totalPages;
	for (; pageCount > n && prevBop > 0; pageCount--) {
		in().seekg(41, ios_base::cur);   // skip bop and 10*4 \count bytes => now on pointer to prev bop
		prevBop = readSigned(4);
		in().seekg(prevBop, ios_base::beg);
	}
	while (pageCount == n && executeCommand() != 140); // 140 == eop
	return true;
}


/** Reads and executes the commands of the postamble. */
void DVIReader::executePostamble () {
	in().clear();  // reset all status bits
	if (!in())
		throw DVIException("invalid DVI file");
	in().seekg(-1, ios_base::end);    // stream pointer to last byte
	while (in().peek() == 223) 
		in().seekg(-1, ios_base::cur); // skip fill bytes

	in().seekg(-4, ios_base::cur);    // now on first byte of q (pointer to begin of postamble)
	UInt32 q = readUnsigned(4);       // pointer to begin of postamble
	in().seekg(q, ios_base::beg);     // now on begin of postamble
	while (executeCommand() != 249);  // read all commands until postpost (= 249) is reached
}


/** Returns the current x coordinate in TeX point units. 
 *  This is the horizontal position where the next output would be placed. */
double DVIReader::getXPos () const {
	return currPos.h;
}

/** Returns the current y coordinate in TeX point units. 
 *  This is the vertical position where the next output would be placed. */
double DVIReader::getYPos () const {
	return currPos.v;
}

double DVIReader::getPageHeight () const {
	return pageHeight;
}

double DVIReader::getPageWidth () const {
	return pageWidth;
}

/////////////////////////////////////		

/** Reads and executes DVI preamble command. */
void DVIReader::cmdPre (int) {
	UInt32 i   = readUnsigned(1);  // identification number (should be 2)
	UInt32 num = readUnsigned(4);  // numerator units of measurement
	UInt32 den = readUnsigned(4);  // denominator units of measurement
	mag = readUnsigned(4);         // magnification
	UInt32 k   = readUnsigned(1);  // length of following comment 
	string cmt = readString(k);    // comment
	if (i != 2)
		throw DVIException("invalid identification value in DVI preamble");
	// 1 dviunit * num/den == multiples of 0.0000001m
	// 1 dviunit * scaleFactor: length of 1 dviunit in TeX points * mag/1000
	scaleFactor = num/25400000.0*7227.0/den*mag/1000.0;
	if (actions)
		actions->preamble(cmt);
}


/** Reads and executes DVI postamble command. */
void DVIReader::cmdPost (int) {
	prevBop    = readUnsigned(4);  // pointer to previous bop
	UInt32 num = readUnsigned(4);  
	UInt32 den = readUnsigned(4);
	mag = readUnsigned(4);
	pageHeight = readUnsigned(4);  // height of tallest page in dvi units
	pageWidth  = readUnsigned(4);  // width of widest page in dvi units
	readUnsigned(2);               // max. stack depth
	totalPages = readUnsigned(2);  // total number of pages
	// 1 dviunit * num/den == multiples of 0.0000001m
	// 1 dviunit * scaleFactor: length of 1 dviunit in TeX points * mag/1000
	scaleFactor = num/25400000.0*7227.0/den*mag/1000.0;
	pageHeight *= scaleFactor;     // to pt units
	pageWidth *= scaleFactor;
	inPostamble = true;
	if (actions)
		actions->postamble();
}


/** Reads and executes DVI postpost command. */
void DVIReader::cmdPostPost (int) {
	inPostamble = false;
	readUnsigned(4);               // pointer to begin of postamble
	UInt32 i = readUnsigned(1);    // identification byte (should be 2)
	if (i == 2) 
		while (readUnsigned(1) == 223);  // skip fill bytes (223), eof bit should be set now
	else 
		throw DVIException("invalid identification value in postpost");
}


/** Reads and executes Begin-Of-Page command. */
void DVIReader::cmdBop (int) {
	Int32 c[10];
	for (int i=0; i < 10; i++)
		c[i] = readSigned(4);
	readSigned(4);        // pointer to peceeding bop (-1 in case of first page)
	currPos.reset();      // set all DVI registers to 0
	while (!posStack.empty())
		posStack.pop();
	currFontNum = 0;
	inPage = true;
	beginPage(c);
	if (actions)
		actions->beginPage(c);
}


/** Reads and executes End-Of-Page command. */
void DVIReader::cmdEop (int) {
	if (!posStack.empty()) 
		throw DVIException("stack not empty at end of page");
	inPage = false;
	endPage();
	if (actions)
		actions->endPage();
}


/** Reads and executes push command. */
void DVIReader::cmdPush (int) {	
	posStack.push(currPos);
}


/** Reads and executes pop command (restores pushed position information). */
void DVIReader::cmdPop (int) {
	if (posStack.empty()) 
		throw DVIException("stack empty at pop command");
	else {
		DVIPosition prevPos = currPos;
		currPos = posStack.top();
		posStack.pop();
		if (actions) {
			if (prevPos.h != currPos.h)
				actions->moveToX(currPos.h);
			if (prevPos.v != currPos.v)
				actions->moveToY(currPos.v);
		}
	}
}


/** Helper function that actually sets/puts a charater. It is called by the
 *  cmdSetChar and cmdPutChar methods.
 * @param[in] c character to typeset
 * @param[in] moveCursor if true, register h is increased by the character width 
 * @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::putChar (UInt32 c, bool moveCursor) {
	if (inPage) {   
		Font *font = fontManager->getFont(currFontNum);
		if (VirtualFont *vf = dynamic_cast<VirtualFont*>(font)) {    // is current font a virtual font?
			vector<UInt8> *dvi = const_cast<vector<UInt8>*>(vf->getDVI(c)); // get DVI snippet that describes character c
			if (dvi) {
				DVIPosition pos = currPos;       // save current cursor position
				currPos.x = currPos.y = currPos.w = currPos.z = 0;
				int save_fontnum = currFontNum;  // save current font number
				fontManager->enterVF(vf);        // new font number context
				cmdFontNum0(fontManager->vfFirstFontNum(vf));
				double save_scale = scaleFactor;
				scaleFactor = vf->scaledSize()/(1 << 20);
				
				VectorInputStream<UInt8> vis(*dvi);
				istream &is = replaceStream(vis);
				try {
					executeAll();            // execute DVI fragment
				}
				catch (const DVIException &e) {
//					Message::estream(true) << "invalid dvi in vf: " << e.getMessage() << endl; // @@
				}
				replaceStream(is);          // restore previous input stream
				scaleFactor = save_scale;   // restore previous scale factor
				fontManager->leaveVF();     // restore previous font number context
				cmdFontNum0(save_fontnum);  // restore previous font number
				currPos = pos;              // restore previous cursor position
			}
		}
		else if (actions) {
			actions->setChar(currPos.h, currPos.v, c, font);
		}
		if (moveCursor)
			currPos.h += font->charWidth(c) * font->scaleFactor() * mag/1000.0;
	}
	else
		throw DVIException("set_char or put_char outside page");
}


/** Reads and executes set_char_x command. Puts a character at the current 
 *  position and advances the cursor. 
 *  @param[in] c character to set 
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdSetChar0 (int c) {
	putChar(c, true);
}


/** Reads and executes setx command. Puts a character at the current 
 *  position and advances the cursor. 
 *  @param[in] len number of parameter bytes (possible values: 1-4)
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdSetChar (int len) {	
   // According to the dvi specification all character codes are unsigned 
   // except len == 4. At the moment all char codes are treated as unsigned...
	UInt32 c = readUnsigned(len); // if len == 4 c may be signed
	putChar(c, true);
}


/** Reads and executes putx command. Puts a character at the current 
 *  position but doesn't change the cursor position. 
 *  @param[in] len number of parameter bytes (possible values: 1-4)
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdPutChar (int len) {
   // According to the dvi specification all character codes are unsigned 
   // except len == 4. At the moment all char codes are treated as unsigned...
	Int32 c = readUnsigned(len);
	putChar(c, false);
}


/** Reads and executes set_rule command. Puts a solid rectangle at the current 
 *  position and updates the cursor position. 
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdSetRule (int) {
	if (inPage) {
		double height = scaleFactor*readSigned(4);
		double width  = scaleFactor*readSigned(4);
		if (actions && height > 0 && width > 0) 
			actions->setRule(currPos.h, currPos.v, height, width);
		currPos.h += width;
		if (actions && (height <= 0 || width <= 0))
			actions->moveToX(currPos.h);	
	}
	else
		throw DVIException("set_rule outside of page");
}


/** Reads and executes set_rule command. Puts a solid rectangle at the current 
 *  position but leaves the cursor position unchanged. 
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdPutRule (int) {
	if (inPage) {
		double height = scaleFactor*readSigned(4);
		double width  = scaleFactor*readSigned(4);
		if (actions && height > 0 && width > 0) 
			actions->setRule(currPos.h, currPos.v, height, width);
	}
	else
		throw DVIException("put_rule outside of page");
}


void DVIReader::cmdNop (int)       {}

void DVIReader::cmdRight (int len) {currPos.h += scaleFactor*readSigned(len); if (actions) actions->moveToX(currPos.h);}
void DVIReader::cmdDown (int len)  {currPos.v += scaleFactor*readSigned(len); if (actions) actions->moveToY(currPos.v);}
void DVIReader::cmdX0 (int)        {currPos.h += currPos.x;                   if (actions) actions->moveToX(currPos.h);}
void DVIReader::cmdY0 (int)        {currPos.v += currPos.y;                   if (actions) actions->moveToY(currPos.v);}
void DVIReader::cmdW0 (int)        {currPos.h += currPos.w;                   if (actions) actions->moveToX(currPos.h);}
void DVIReader::cmdZ0 (int)        {currPos.v += currPos.z;                   if (actions) actions->moveToY(currPos.v);}
void DVIReader::cmdX (int len)     {currPos.x = scaleFactor*readSigned(len);  cmdX0(0);}
void DVIReader::cmdY (int len)     {currPos.y = scaleFactor*readSigned(len);  cmdY0(0);}
void DVIReader::cmdW (int len)     {currPos.w = scaleFactor*readSigned(len);  cmdW0(0);}
void DVIReader::cmdZ (int len)     {currPos.z = scaleFactor*readSigned(len);  cmdZ0(0);}


void DVIReader::cmdXXX (int len) {
	if (inPage) {
		UInt32 numBytes = readUnsigned(len);
		string s = readString(numBytes);
		if (actions)
			actions->special(s);
	}
	else
		throw DVIException("special outside of page");
}


/** Selects a previously defined font by its number. 
 * @param[in] num font number 
 * @throw DVIException if font number is undefined */
void DVIReader::cmdFontNum0 (int num) {
	if (Font *font = fontManager->getFont(num)) {
		currFontNum = num;
		if (actions && !dynamic_cast<VirtualFont*>(font))
			actions->setFont(fontManager->fontID(num), font);  // all actions get a recomputed font number
	}
	else {
		ostringstream oss;
		oss << "undefined font number " << num;
		throw DVIException(oss.str());
	}
}


/** Selects a previously defined font.
 * @param[in] len size of font number variable (in bytes) 
 * @throw DVIException if font number is undefined */
void DVIReader::cmdFontNum (int len) {
	UInt32 num = readUnsigned(len);
	cmdFontNum0(num);	
}


/** Helper function to handle a font definition.
 *  @param[in] fontnum local font number 
 *  @param[in] name font name
 *  @param[in] checksum checksum to be compared with TFM checksum 
 *  @param[in] ds design size in TeX point units
 *  @param[in] ss scaled size in TeX point units */
void DVIReader::defineFont (UInt32 fontnum, const string &name, UInt32 cs, double ds, double ss) {
	if (fontManager) {
		int id = fontManager->registerFont(fontnum, name, cs, ds, ss);
		Font *font = fontManager->getFontById(id);
		if (VirtualFont *vf = dynamic_cast<VirtualFont*>(font)) {
			// read vf file, register its font and character definitions
			fontManager->enterVF(vf);
			ifstream ifs(vf->path(), ios::binary);
			VFReader vfReader(ifs);
			vfReader.replaceActions(this);
			vfReader.executeAll();
			fontManager->leaveVF();
		}
		if (actions)
			actions->defineFont(id, font);
	}
}


/** Defines a new font. 
 * @param[in] len size of font number variable (in bytes) */
void DVIReader::cmdFontDef (int len) {
	UInt32 fontnum  = readUnsigned(len);   // font number
	UInt32 checksum = readUnsigned(4);     // font checksum (to be compared with corresponding TFM checksum)
	UInt32 ssize    = readUnsigned(4);     // scaled size of font in DVI units
	UInt32 dsize    = readUnsigned(4);     // design size of font in DVI units
	UInt32 pathlen  = readUnsigned(1);     // length of font path
	UInt32 namelen  = readUnsigned(1);     // length of font name
	string fontpath = readString(pathlen);
	string fontname = readString(namelen);
	
	defineFont(fontnum, fontname, checksum, dsize*scaleFactor, ssize*scaleFactor);
}


/** This template method is called by the VFReader after reading a font definition from a VF file.
 *  @param[in] fontnum local font number 
 *  @param[in] name font name
 *  @param[in] checksum checksum to be compared with TFM checksum 
 *  @param[in] dsize design size in TeX point units
 *  @param[in] ssize scaled size in TeX point units */
void DVIReader::defineVFFont (UInt32 fontnum, string path, string name, UInt32 checksum, double dsize, double ssize) {
	VirtualFont *vf = fontManager->getVF();	
	defineFont(fontnum, name, checksum, dsize, ssize * vf->scaleFactor());
}


/** This template method is called by the VFReader after reading a character definition from a VF file.
 *  @param[in] c character number
 *  @param[in] dvi DVI fragment describing the character */
void DVIReader::defineVFChar (UInt32 c, vector<UInt8> *dvi) {
	fontManager->assignVfChar(c, dvi);
}
