/*************************************************************************
** DVIReader.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

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
#include "SignalHandler.h"
#include "VectorStream.h"
#include "macros.h"

using namespace std;

bool DVIReader::COMPUTE_PAGE_LENGTH = false;


DVIReader::DVIReader (istream &is, DVIActions *a) : StreamReader(is), _actions(a)
{
	_inPage = false;
	_pageHeight = _pageWidth = 0;
	_scaleFactor = 0.0;
	_tx = _ty = 0;    // no cursor translation
	_prevYPos = numeric_limits<double>::min();
	_inPostamble = false;
	_totalPages = 0;  // we don't know the correct value yet
	_currFontNum = 0;
	_currPageNum = 0;
	_pageLength = 0;
	_pagePos = 0;
}


DVIActions* DVIReader::replaceActions (DVIActions *a) {
	DVIActions *prev_actions = _actions;
	_actions = a;
	return prev_actions;
}


/** Evaluates the next DVI command, and computes the corresponding handler.
 *  @param[in] compute_size if true, the size of variable-length parameters is computed
 *  @param[out] handler handler for current DVI command
 *  @param[out] number of parameter bytes
 *  @param[out] param the handler must be called with this parameter
 *  @return opcode of current DVI command */
int DVIReader::evalCommand (bool compute_size, CommandHandler &handler, int &length, int &param) {
	struct DVICommand {
		CommandHandler handler;
		int length;  // number of parameter bytes
	};

   /* Each cmdFOO command reads the necessary number of bytes from the stream, so executeCommand
   doesn't need to know the exact DVI command format. Some cmdFOO methods are used for multiple
	DVI commands because they only differ in length of their parameters. */
	static const DVICommand commands[] = {
		{&DVIReader::cmdSetChar, 1}, {&DVIReader::cmdSetChar, 2}, {&DVIReader::cmdSetChar, 3}, {&DVIReader::cmdSetChar, 4}, // 128-131
		{&DVIReader::cmdSetRule, 8},                                                                                        // 132
		{&DVIReader::cmdPutChar, 1}, {&DVIReader::cmdPutChar, 2}, {&DVIReader::cmdPutChar, 3}, {&DVIReader::cmdPutChar, 4}, // 133-136
		{&DVIReader::cmdPutRule, 8},                                                                                        // 137
		{&DVIReader::cmdNop, 0},                                                                                            // 138
		{&DVIReader::cmdBop, 44},    {&DVIReader::cmdEop, 0},                                                               // 139-140
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

	const int opcode = in().get();
	if (!in() || opcode < 0)  // at end of file
		throw InvalidDVIFileException("invalid file");

	param = -1;
	if (opcode >= 0 && opcode <= 127) {
		handler = &DVIReader::cmdSetChar0;
		length = 0;
		param = opcode;
	}
	else if (opcode >= 171 && opcode <= 234) {
		handler = &DVIReader::cmdFontNum0;
		length = 0;
		param = opcode-171;
	}
	else if (opcode >= 250) {
		ostringstream oss;
		oss << "undefined DVI command (opcode " << opcode << ')';
		throw DVIException(oss.str());
	}
	else {
		const int offset = opcode <= 170 ? 128 : 235-(170-128+1);
		handler = commands[opcode-offset].handler;
		if (!compute_size)
			length = commands[opcode-offset].length;
		else {
			if (opcode >= 239 && opcode <= 242) { // specials
				int len = opcode-238;
				UInt32 bytes = readUnsigned(len);
				in().seekg(-len, ios_base::cur);
				length = len+bytes;
			}
			else if (opcode >= 243 && opcode <= 246) { // fontdefs
				int len = opcode-242;
				len += 12;
				in().seekg(len, ios_base::cur);   // skip fontnum, checksum, ssize, dsize
				UInt32 bytes = readUnsigned(1);   // length of font path
				bytes += readUnsigned(1);         // length of font name
				in().seekg(-len-2, ios_base::cur);
				length = len+bytes;
			}
			else
				length = commands[opcode-offset].length;
		}
	}
	if (param < 0)
		param = length;
	return opcode;
}


/** Reads a single DVI command from the current position of the input stream and calls the
 *  corresponding cmdFOO method.
 *  @return opcode of the executed command */
int DVIReader::executeCommand () {
	SignalHandler::instance().check();
	CommandHandler handler;
	int len;   // number of parameter bytes
	int param; // parameter of handler
	streampos pos = in().tellg();
	int opcode = evalCommand(false, handler, len, param);
	(this->*handler)(param);
	if (_currPos.v+_ty != _prevYPos) {
		_tx = _ty = 0;
		_prevYPos = _currPos.v;
	}
	if (COMPUTE_PAGE_LENGTH && _inPage && _actions) {
		// ensure progress() is called at 0%
		if (opcode == 139) // bop?
			_actions->progress(0, _pageLength);
		// ensure progress() is called at 100%
		if (in().peek() == 140)  // eop reached?
			_pagePos = _pageLength;
		else
			_pagePos += in().tellg()-pos;
		_actions->progress(_pagePos, _pageLength);
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


#if 0
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
#endif


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
	if (n < 1 || n > _totalPages)
		return false;
	in().seekg(_prevBop, ios_base::beg); // now on last bop
	_inPostamble = false;               // we jumped out of the postamble
	unsigned pageCount = _totalPages;
	for (; pageCount > n && _prevBop > 0; pageCount--) {
		in().seekg(41, ios_base::cur);   // skip bop and 10*4 \count bytes => now on pointer to prev bop
		_prevBop = readSigned(4);
		in().seekg(_prevBop, ios_base::beg);
	}
	_currPageNum = n;
	while (pageCount == n && executeCommand() != 140); // 140 == eop
	return true;
}


bool DVIReader::executePages (unsigned first, unsigned last) {
	in().clear();
	if (!in())
		throw DVIException("invalid DVI file");
	if (first > last)
		swap(first, last);
	in().seekg(-1, ios_base::end);      // stream pointer to last byte
	while (in().peek() == 223)
		in().seekg(-1, ios_base::cur);   // skip fill bytes
	in().seekg(-4, ios_base::cur);      // now on first byte of q (pointer to begin of postamble)
	UInt32 q = readUnsigned(4);         // pointer to begin of postamble
	in().seekg(q, ios_base::beg);       // now on begin of postamble
	if (executeCommand() != 248)        // execute postamble command but not the fontdefs
		return false;
	first = max(1u, min(first, _totalPages));
	last = max(1u, min(last, _totalPages));
	in().seekg(_prevBop, ios_base::beg); // now on last bop
	_inPostamble = false;               // we jumped out of the postamble
	unsigned count = _totalPages;
	for (; count > first && _prevBop > 0; count--) {
		in().seekg(41, ios_base::cur);   // skip bop and 10*4 \count bytes => now on pointer to prev bop
		_prevBop = readSigned(4);
		in().seekg(_prevBop, ios_base::beg);
	}
	while (first <= last) {
		_currPageNum = first++;
		while (executeCommand () != 140); // 140 == eop
	}
	return true;
}


void DVIReader::executePreamble () {
	in().clear();
	if (!in())
		throw DVIException("invalid DVI file");
	in().seekg(0, ios_base::beg);
	if (in().get() != 247)
		throw DVIException("invalid DVI file");
	cmdPre(0);
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
	return _currPos.h+_tx;
}


/** Returns the current y coordinate in TeX point units.
 *  This is the vertical position where the next output would be placed. */
double DVIReader::getYPos () const {
	return _currPos.v+_ty;
}


double DVIReader::getPageHeight () const {
	return _pageHeight;
}


double DVIReader::getPageWidth () const {
	return _pageWidth;
}

/////////////////////////////////////

/** Reads and executes DVI preamble command. */
void DVIReader::cmdPre (int) {
	UInt32 i   = readUnsigned(1);  // identification number (should be 2)
	UInt32 num = readUnsigned(4);  // numerator units of measurement
	UInt32 den = readUnsigned(4);  // denominator units of measurement
	_mag = readUnsigned(4);        // magnification
	UInt32 k   = readUnsigned(1);  // length of following comment
	string cmt = readString(k);    // comment
	if (i != 2) {
		ostringstream oss;
		oss << "DVI format version " << i << " not supported";
		throw DVIException(oss.str());
	}
	// 1 dviunit * num/den == multiples of 0.0000001m
	// 1 dviunit * _scaleFactor: length of 1 dviunit in TeX points * _mag/1000
	_scaleFactor = num/25400000.0*7227.0/den*_mag/1000.0;
	if (_actions)
		_actions->preamble(cmt);
}


/** Reads and executes DVI postamble command. */
void DVIReader::cmdPost (int) {
	_prevBop   = readUnsigned(4);  // pointer to previous bop
	UInt32 num = readUnsigned(4);
	UInt32 den = readUnsigned(4);
	_mag = readUnsigned(4);
	_pageHeight = readUnsigned(4); // height of tallest page in dvi units
	_pageWidth  = readUnsigned(4); // width of widest page in dvi units
	readUnsigned(2);               // max. stack depth
	_totalPages = readUnsigned(2); // total number of pages
	// 1 dviunit * num/den == multiples of 0.0000001m
	// 1 dviunit * _scaleFactor: length of 1 dviunit in TeX points * _mag/1000
	_scaleFactor = num/25400000.0*7227.0/den*_mag/1000.0;
	_pageHeight *= _scaleFactor;     // to pt units
	_pageWidth *= _scaleFactor;
	_inPostamble = true;
	if (_actions)
		_actions->postamble();
}


/** Reads and executes DVI postpost command. */
void DVIReader::cmdPostPost (int) {
	_inPostamble = false;
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
	_currPos.reset();     // set all DVI registers to 0
	while (!_posStack.empty())
		_posStack.pop();
	_currFontNum = 0;
	_inPage = true;
	_pageLength = 0;
	_pagePos = 0;
	if (COMPUTE_PAGE_LENGTH) {
		// compute number of bytes in current page
		int length, param;
		CommandHandler handler;
		// read all commands until eop is found
		while (evalCommand(true, handler, length, param) != 140) {
			in().seekg(length, ios_base::cur);
			_pageLength += length+1;  // parameter length + opcode length (1 byte)
		}
		++_pageLength;  // add length of eop command (1 byte)
		in().seekg(-int(_pageLength), ios_base::cur);  // go back to first command following bop
		_pageLength += 45; // add length of bop command
	}
	beginPage(_currPageNum, c);
	if (_actions)
		_actions->beginPage(_currPageNum, c);
}


/** Reads and executes End-Of-Page command. */
void DVIReader::cmdEop (int) {
	if (!_posStack.empty())
		throw DVIException("stack not empty at end of page");
	_inPage = false;
	endPage();
	if (_actions)
		_actions->endPage();
}


/** Reads and executes push command. */
void DVIReader::cmdPush (int) {
	_posStack.push(_currPos);
}


/** Reads and executes pop command (restores pushed position information). */
void DVIReader::cmdPop (int) {
	if (_posStack.empty())
		throw DVIException("stack empty at pop command");
	else {
		DVIPosition prevPos = _currPos;
		_currPos = _posStack.top();
		_posStack.pop();
		if (_actions) {
			if (prevPos.h != _currPos.h)
				_actions->moveToX(_currPos.h + _tx);
			if (prevPos.v != _currPos.v)
				_actions->moveToY(_currPos.v + _ty);
		}
	}
}


/** Helper function that actually sets/puts a charater. It is called by the
 *  cmdSetChar and cmdPutChar methods.
 * @param[in] c character to typeset
 * @param[in] moveCursor if true, register h is increased by the character width
 * @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::putChar (UInt32 c, bool moveCursor) {
	if (!_inPage)
		throw DVIException("set_char or put_char outside of page");

	FontManager &fm = FontManager::instance();
	Font *font = fm.getFont(_currFontNum);
	if (!font)
		throw DVIException("no font selected");

	if (VirtualFont *vf = dynamic_cast<VirtualFont*>(font)) {    // is current font a virtual font?
		vector<UInt8> *dvi = const_cast<vector<UInt8>*>(vf->getDVI(c)); // get DVI snippet that describes character c
		if (dvi) {
			DVIPosition pos = _currPos;       // save current cursor position
			_currPos.x = _currPos.y = _currPos.w = _currPos.z = 0;
			int save_fontnum = _currFontNum; // save current font number
			fm.enterVF(vf);        // new font number context
			cmdFontNum0(fm.vfFirstFontNum(vf));
			double save_scale = _scaleFactor;
			_scaleFactor = vf->scaledSize()/(1 << 20);

			VectorInputStream<UInt8> vis(*dvi);
			istream &is = replaceStream(vis);
			try {
				executeAll();            // execute DVI fragment
			}
			catch (const DVIException &e) {
				//					Message::estream(true) << "invalid dvi in vf: " << e.getMessage() << endl; // @@
			}
			replaceStream(is);          // restore previous input stream
			_scaleFactor = save_scale;  // restore previous scale factor
			fm.leaveVF();               // restore previous font number context
			cmdFontNum0(save_fontnum);  // restore previous font number
			_currPos = pos;             // restore previous cursor position
		}
	}
	else if (_actions) {
		_actions->setChar(_currPos.h+_tx, _currPos.v+_ty, c, font);
	}
	if (moveCursor)
		_currPos.h += font->charWidth(c) * font->scaleFactor() * _mag/1000.0;
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
	if (_inPage) {
		double height = _scaleFactor*readSigned(4);
		double width  = _scaleFactor*readSigned(4);
		if (_actions && height > 0 && width > 0)
			_actions->setRule(_currPos.h+_tx, _currPos.v+_ty, height, width);
		_currPos.h += width;
		if (_actions && (height <= 0 || width <= 0))
			_actions->moveToX(_currPos.h+_tx);
	}
	else
		throw DVIException("set_rule outside of page");
}


/** Reads and executes set_rule command. Puts a solid rectangle at the current
 *  position but leaves the cursor position unchanged.
 *  @throw DVIException if method is called ouside a bop/eop pair */
void DVIReader::cmdPutRule (int) {
	if (_inPage) {
		double height = _scaleFactor*readSigned(4);
		double width  = _scaleFactor*readSigned(4);
		if (_actions && height > 0 && width > 0)
			_actions->setRule(_currPos.h+_tx, _currPos.v+_ty, height, width);
	}
	else
		throw DVIException("put_rule outside of page");
}


void DVIReader::cmdNop (int)       {}

void DVIReader::cmdRight (int len) {_currPos.h += _scaleFactor*readSigned(len); if (_actions) _actions->moveToX(_currPos.h+_tx);}
void DVIReader::cmdDown (int len)  {_currPos.v += _scaleFactor*readSigned(len); if (_actions) _actions->moveToY(_currPos.v+_ty);}
void DVIReader::cmdX0 (int)        {_currPos.h += _currPos.x;                   if (_actions) _actions->moveToX(_currPos.h+_tx);}
void DVIReader::cmdY0 (int)        {_currPos.v += _currPos.y;                   if (_actions) _actions->moveToY(_currPos.v+_ty);}
void DVIReader::cmdW0 (int)        {_currPos.h += _currPos.w;                   if (_actions) _actions->moveToX(_currPos.h+_tx);}
void DVIReader::cmdZ0 (int)        {_currPos.v += _currPos.z;                   if (_actions) _actions->moveToY(_currPos.v+_ty);}
void DVIReader::cmdX (int len)     {_currPos.x = _scaleFactor*readSigned(len);  cmdX0(0);}
void DVIReader::cmdY (int len)     {_currPos.y = _scaleFactor*readSigned(len);  cmdY0(0);}
void DVIReader::cmdW (int len)     {_currPos.w = _scaleFactor*readSigned(len);  cmdW0(0);}
void DVIReader::cmdZ (int len)     {_currPos.z = _scaleFactor*readSigned(len);  cmdZ0(0);}


void DVIReader::cmdXXX (int len) {
	if (_inPage) {
		UInt32 numBytes = readUnsigned(len);
		string s = readString(numBytes);
		if (_actions)
			_actions->special(s);
	}
	else
		throw DVIException("special outside of page");
}


/** Selects a previously defined font by its number.
 * @param[in] num font number
 * @throw DVIException if font number is undefined */
void DVIReader::cmdFontNum0 (int num) {
	if (Font *font = FontManager::instance().getFont(num)) {
		_currFontNum = num;
		if (_actions && !dynamic_cast<VirtualFont*>(font))
			_actions->setFont(FontManager::instance().fontID(num), font);  // all fonts get a recomputed ID
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
   FontManager &fm = FontManager::instance();
	int id = fm.registerFont(fontnum, name, cs, ds, ss);
	Font *font = fm.getFontById(id);
	if (VirtualFont *vf = dynamic_cast<VirtualFont*>(font)) {
		// read vf file, register its font and character definitions
		fm.enterVF(vf);
		ifstream ifs(vf->path(), ios::binary);
		VFReader vfReader(ifs);
		vfReader.replaceActions(this);
		vfReader.executeAll();
		fm.leaveVF();
	}
	if (_actions)
		_actions->defineFont(id, font);
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

	defineFont(fontnum, fontname, checksum, dsize*_scaleFactor, ssize*_scaleFactor);
}


/** This template method is called by the VFReader after reading a font definition from a VF file.
 *  @param[in] fontnum local font number
 *  @param[in] name font name
 *  @param[in] checksum checksum to be compared with TFM checksum
 *  @param[in] dsize design size in TeX point units
 *  @param[in] ssize scaled size in TeX point units */
void DVIReader::defineVFFont (UInt32 fontnum, string path, string name, UInt32 checksum, double dsize, double ssize) {
	VirtualFont *vf = FontManager::instance().getVF();
	defineFont(fontnum, name, checksum, dsize, ssize * vf->scaleFactor());
}


/** This template method is called by the VFReader after reading a character definition from a VF file.
 *  @param[in] c character number
 *  @param[in] dvi DVI fragment describing the character */
void DVIReader::defineVFChar (UInt32 c, vector<UInt8> *dvi) {
	FontManager::instance().assignVfChar(c, dvi);
}
