/***********************************************************************
** FontEncoding.cpp                                                   **
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
// $Id$

#include <fstream>
#include "debug.h"
#include "FontEncoding.h"
#include "InputBuffer.h"
#include "KPSFileFinder.h"
#include "Message.h"

using namespace std;

static string read_entry (InputBuffer &in);
static bool valid_name_char (char c);


FontEncoding::FontEncoding (const string &encname) : _encname(encname) 
{
}



/** Search for suitable enc-file and read its encoding information. 
 *  The file contents must be a valid PostScript vector with 256 entries. */
void FontEncoding::read () {
	const char *path = KPSFileFinder::lookup(_encname+".enc");
	if (path) {
		ifstream ifs(path);
		read(ifs);
	}
	else
		Message::mstream(true) << "encoding file '" << _encname << ".enc' not found";
}


/** Read encoding information from stream. */
void FontEncoding::read (istream &is) {
	StreamInputBuffer in(is, 256);
	_table.resize(256);
	
	// find beginning of vector
	while (!in.eof()) {
		in.skipSpace();
		if (in.peek() == '%') 
			in.skipUntil("\n");
		else 
			if (in.get() == '[')
				break;
	}
	
	// read vector entries
	int n=0;
	while (!in.eof()) {
		in.skipSpace();
		if (in.peek() == '%') 
			in.skipUntil("\n");
		else if (in.peek() == ']') {
			in.get();
			break;
		}
		else {
			string entry = read_entry(in);
			if (entry == ".notdef")
				entry = "";
			if (entry.length() > 0 && n < 256)
				_table[n++] = entry;
		}
	}
	// remove trailing .notdef names
	for (n--; n > 0 && _table[n] == ""; n--);	
	_table.resize(n+1);
	for (unsigned i=0; i < _table.size(); i++)
		cout << i << ": " << _table[i] << endl;  // @@
}


static string read_entry (InputBuffer &in) {
	string entry;
	bool accept_slashes=true;
	while (!in.eof() && ((in.peek() == '/' && accept_slashes) || valid_name_char(in.peek()))) {
		if (in.peek() != '/')
			accept_slashes = false;
		entry += in.get();
	}
	if (entry.length() > 1) {
		// strip leading slashes 
		// According to the PostScript specification, a single slash without further 
		// following characters is a valid name.
		size_t n=0;
		while (n < entry.length() && entry[n] == '/')
			n++;
		entry = entry.substr(n);
	}
	return entry;
}


static bool valid_name_char (char c) {
	const char *delimiters = "<>(){}[]/~%";
	return isprint(c) && !isspace(c) && !strchr(delimiters, c);
}


string FontEncoding::getEntry (UInt32 c) const {
	if (c >= 0 && c < _table.size())
		return _table[c];
	return 0;
}

