/***********************************************************************
** FontMap.cpp                                                        **
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
#include <iostream>
#include <vector>
#include "Directory.h"
#include "FontMap.h"

using namespace std;


/** Helper function: splits a given line of text into several parts. 
 * @param[in]  str pointer to line buffer (must be writable) 
 * @param[out] parts the parts are written to this vector 
 * @param[in]  max_parts maximal number of parts to be stored (0 = no limit) 
 * @return number of extracted parts */
static int split (char *str, vector<string> *parts, unsigned max_parts=0) {
	if (str && parts) {
		parts->clear();
		while (*str && (max_parts == 0 || parts->size() < max_parts)) {
			char *l = str;
			while (*l == ' ' || *l == '\t')  // skip leading spaces
				l++;
			if (*l && *l != '%') {  // begin of entry found?
				char *r = l;
				while (*r && *r != ' ' && *r != '\t')
					r++;
				str = r;
				if (*r) {		   // end of entry found?		
					*r = 0;        // mark end of entry
					str = r+1;     // next possible entry starts here
				}
				parts->push_back(l);
			}
			else
				*str = 0;         // terminate
		}
		return parts->size();
	}
	return 0;
}


/** Strips dvipdfm map-file options off previously collected 'parts' 
 * @param[in,out] parts vector of strings
 * @return number of remaining elements in vector */
static int remove_options (vector<string> &parts) {
	vector<string>::iterator it=parts.begin();
	while (it != parts.end() && (*it)[0] != '-')
		++it;
	while (it != parts.end())
		parts.erase(it);
	return parts.size();
}


FontMap::FontMap (istream &is) {
	read(is);
}


/** Reads the font mapping information from the given stream. 
 *  The information must be given in the map-file format of dvipdfm:
 *  <font name> [<encoding>|default|none] [<map target>] [options]
 *  The optional dvipdfm-parameters -r, -e and -s are ignored. 
 *  @param[in] is map file data is read from this stream */
void FontMap::read (istream &is) {
	char buf[256];
	while (is) {
		is.getline(buf, 256);
		vector<string> parts;
		split(buf, &parts, 3);
		if (remove_options(parts) < 2)
			continue;
	
		if (parts[1] == "default" || parts[1] == "none")
			parts[1] = "";
		if ((parts.size() == 2 && parts[1] == "") || (parts.size() == 3 && parts[1] == "" && parts[0] == parts[2]))
			continue;

		_fontMap[parts[0]].fontname = parts[parts.size() == 2 ? 0 : 2];
		_fontMap[parts[0]].encname  = parts[1];
	}
}


ostream& FontMap::write (ostream &os) const {
	for (map<string,MapEntry>::const_iterator i=_fontMap.begin(); i != _fontMap.end(); ++i)
		os << i->first << " -> " << i->second.fontname << endl;
	return os;
}


void FontMap::readdir (const string &dirname) {
	Directory dir(dirname);
	while (const char *fname = dir.read('f')) {
		if (strlen(fname) >= 4 && strcmp(fname+strlen(fname)-4, ".map") == 0) {
			string path = dirname + "/" + fname;
			ifstream ifs(path.c_str());
			read(ifs);
		}
	}
}


/** Returns name of font that is mapped to a given font. 
 * @param[in] fontname name of font whose mapped name is retrieved
 * @returns name of mapped font */
const char* FontMap::lookup (const string &fontname) const {
	ConstIterator it = _fontMap.find(fontname);
	if (it == _fontMap.end())
		return 0;
	return it->second.fontname.c_str();
}


/** Returns the name of the assigned encoding for a given font. 
 *  @param[in] fontname name of font whose encoding is returned
 *  @return name of encoding, 0 if there is no encoding assigned */
const char* FontMap::encoding (const string &fontname) const {
	ConstIterator it = _fontMap.find(fontname);
	if (it == _fontMap.end() || it->second.encname == "")
		return 0;
	return it->second.encname.c_str();
}
