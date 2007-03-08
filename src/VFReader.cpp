/***********************************************************************
** VFReader.cpp                                                      **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id$

#include <sstream>
#include "FontManager.h"
#include "VFActions.h"
#include "VFReader.h"

using namespace std;


VFReader::VFReader (istream &is) 
	: in(is), actions(0), fontManager(0) {
}


VFReader::~VFReader () {
}


VFActions* VFReader::replaceActions (VFActions *a) {
	VFActions *ret = actions;
	actions = a;
	return ret;
}


/** Reads an unsigned integer from assigned input stream. 
 *  @param bytes number of bytes to read (max. 4)
 *  @return read integer */
UInt32 VFReader::readUnsigned (int bytes) {
	UInt32 ret = 0;
	for (int i=bytes-1; i >= 0 && !in.eof(); i--) {
		UInt32 b = in.get();
		ret |= b << (8*i);
	}
	return ret;
}


/** Reads an signed integer from assigned input stream. 
 *  @param bytes number of bytes to read (max. 4)
 *  @return read integer */
Int32 VFReader::readSigned (int bytes) {
	Int32 ret = in.get();
	if (ret & 128)        // negative value?
		ret |= 0xffffff00;
	for (int i=bytes-2; i >= 0 && !in.eof(); i--) 
		ret = (ret << 8) | in.get();
	return ret;
}


string VFReader::readString (int length) {
	char *buf = new char[length+1];
	if (length > 0)
		in.get(buf, length+1);  // reads 'bytes' bytes (pos. length+1 is set to 0)
	else
		*buf = 0;
	string ret = buf;
	delete [] buf;
	return ret;
}

UInt8* VFReader::readBytes (int n, UInt8 *buf) {
	if (n > 0)
		in.read((char*)buf, n);
	return buf;
}


/** Reads a single VF command from the current position of the input stream and calls the
 *  corresponding cmdFOO method.  
 *  @return opcode of the executed command */
int VFReader::executeCommand () {
	int opcode = in.get();
	if (!in || opcode < 0)  // at end of file
		throw VFException("invalid file");

	if (opcode >= 0 && opcode <= 241)
		cmdShortChar(opcode);
	else if (opcode >= 243 && opcode <= 246)
		cmdFontDef(opcode-243+1);
	else
		switch (opcode) {
			case 242: cmdLongChar(); break;
			case 247: cmdPre();      break;
			case 248: cmdPost();     break;
			default : {
				ostringstream oss; 
				oss << "undefined VF command (opcode " << opcode << ')';
				throw VFException(oss.str());
			}
		}
	return opcode;
}

bool VFReader::executeAll () {
  	in.clear();   // reset all status bits
	if (!in)
		return false;
	in.seekg(0);  // move file pointer to first byte of the input stream
	while (!in.eof() && executeCommand() != 248); // stop reading after post (248)
	return true;

}

//////////////////////////////////////////////////////////////////////////////

/** Reads and executes DVI preamble command. */
void VFReader::cmdPre () {
	UInt32 i   = readUnsigned(1);  // identification number (should be 2)
	UInt32 k   = readUnsigned(1);  // length of following comment 
	string cmt = readString(k);    // comment
	UInt32 cs  = readUnsigned(4);  // check sum to be compared with TFM cecksum
	UInt32 ds  = readUnsigned(4);  // design size (same as TFM design size)
	if (i != 202)
		throw VFException("invalid identification value in preamble");
   if (actions)
      actions->preamble(cmt, cs, ds);
}


void VFReader::cmdPost () {
	UInt32 byte;
	while ((readUnsigned(1)) == 248); // skip fill bytes
	if (actions)
		actions->postamble();
}


void VFReader::cmdLongChar () {
	UInt32 pl  = readUnsigned(4); // packet length (length of DVI subroutine)
	UInt32 cc  = readUnsigned(4); // character code
	UInt32 tfm = readUnsigned(4); // character width from corresponding TFM file
	UInt8 *dvi = new UInt8[pl];   // DVI subroutine
   if (actions)
   	actions->defineChar(cc, dvi);          // call template method for user actions
}


/** Reads and executes short_char_x command.
 *  @param pl packet length (length of DVI subroutine) */
void VFReader::cmdShortChar (int pl) {
	UInt32 cc  = readUnsigned(1); // character code
	UInt32 tfm = readUnsigned(3); // character width from corresponding TFM file
	UInt8 *dvi = new UInt8[pl];   // DVI subroutine
   if (actions)
   	actions->defineChar(cc, dvi);          // call template method for user actions
}


void VFReader::cmdFontDef (int len) {
	UInt32 fontnum  = readUnsigned(len);   // font number
	UInt32 checksum = readUnsigned(4);     // font checksum (to be compared with corresponding TFM checksum)
	UInt32 ssize    = readUnsigned(4);     // scaled size of font relative to design size
	UInt32 dsize    = readUnsigned(4);     // design size of font (same as TFM design size)
	UInt32 pathlen  = readUnsigned(1);     // length of font path
	UInt32 namelen  = readUnsigned(1);     // length of font name
	string fontpath = readString(pathlen);
	string fontname = readString(namelen);
   if (fontManager)
      fontManager->registerFont(fontnum, fontname, checksum, dsize, ssize);
   if (actions)
      actions->defineFont(fontnum, fontname, checksum, dsize, ssize);
}

