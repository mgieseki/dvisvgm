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


FontEncoding::FontEncoding (const string &name) : _name(name) 
{
}


FontEncoding::~FontEncoding () {
}

/** Search for suitable enc-file and read its encoding information. */
void FontEncoding::read () {
	const char *path = KPSFileFinder::lookup(_name+".enc");
	if (path) {
		ifstream ifs(path);
		read(ifs);
	}
	else
		Message::mstream(true) << "encoding file '" << _name << ".enc' not found";
}


/** Read encoding information from stream. */
void FontEncoding::read (istream &is) {
	StreamInputBuffer in(is, 256);
	bool loop = true;
	while (!in.eof() && loop) {
		in.skipSpace();
		if (in.peek() == '%') 
			in.skipUntil("\n");
		else
			loop = false;
	}
}


UInt32 FontEncoding::decode (UInt32 c) const {
	if (c >= 0 && c < _table.size())
		return _table[c];
	return 0;
}

