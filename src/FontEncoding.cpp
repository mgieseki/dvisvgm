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

#include <fstream>
#include "FontEncoding.h"
#include "InputBuffer.h"
#include "InputReader.h"
#include "FileFinder.h"
#include "Message.h"

using namespace std;

static string read_entry (InputReader &in);
static bool valid_name_char (char c);


FontEncoding::FontEncoding (const string &encname) : _encname(encname) 
{
	read();
}


const char* FontEncoding::path () const {
	return FileFinder::lookup(_encname+".enc");
}



/** Search for suitable enc-file and read its encoding information. 
 *  The file contents must be a valid PostScript vector with 256 entries. */
void FontEncoding::read () {
	if (const char *p = path()) {
		ifstream ifs(p);
		read(ifs);
	}
	else
		Message::mstream(true) << "encoding file '" << _encname << ".enc' not found\n";
}


/** Read encoding information from stream. */
void FontEncoding::read (istream &is) {
	StreamInputBuffer ib(is, 256);
	BufferInputReader in(ib);
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
				entry.clear();
			if (n < 256)
				_table[n++] = entry;
		}
	}
	// remove trailing .notdef names
	for (n--; n > 0 && _table[n].empty(); n--);	
	_table.resize(n+1);
}


static string read_entry (InputReader &in) {
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


/** Returns an entry of the encoding table.
 * @param[in] c character code 
 * @return character name assigned to charcter code c*/
const char* FontEncoding::getEntry (int c) const {
	if (c >= 0 && (size_t)c < _table.size())
		return !_table[c].empty() ? _table[c].c_str() : 0;
	return 0;
}

