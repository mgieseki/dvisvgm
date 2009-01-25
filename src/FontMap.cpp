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

enum FontMapFieldType {FM_ERROR=0, FM_NONE, FM_NAME, FM_PS_CODE, FM_HEADER, FM_ENC, FM_FONT};


static char* str_tolower (char *str) {
	for (char *p=str; *p; p++)
		*p = tolower(*p);
	return str;
}


static void skip_space (char* &str) {
	while (*str && isspace(*str))
		str++;
}


static inline char* get_extension (char *fname) {
	if (char* p=strrchr(fname, '.'))
		return p+1;
	return 0;
}


static FontMapFieldType read_entry (char* &first, char* &last, bool name_only=false) {
	skip_space(first);
	if (!name_only) {
		if (*first == '"') {
			last = first+1;
			while (*last && *last != '"')
				last++;
			if (*last == 0)  // quote not closed => skip invalid line
				return FM_ERROR;
			return FM_PS_CODE;
		}
		else if (*first == '<') {
			bool eval_prefix = true;
			FontMapFieldType type=FM_HEADER;
			first++;
			if (isspace(*first)) {
				first++;
				eval_prefix = false;
			}
			else if (*first == '<' || *first == '[') {
				type = (*first == '<') ? FM_FONT : FM_ENC;
				first++;
			}
			if (read_entry(first, last, true) != FM_NAME)
				return FM_ERROR;
			if (type == FM_HEADER) {
				const char *ext = str_tolower(get_extension(first));
				if (strcmp(ext, "enc")==0)
					return FM_ENC;
				if (strcmp(ext, "pfb")==0 || strcmp(ext, "pfa")==0)
					return FM_FONT;
			}
			return type;
		}
	}
	last = first;
	while (*last && !isspace(*last))
		last++;
	*last = 0;
	return (first < last) ? FM_NAME : FM_NONE;
}


void FontMap::readPsMap (istream &is) {
	char buf[512];
	while (is && !is.eof()) {
		is.getline(buf, 512);
		if (!buf[0] || strchr(" %#;*", buf[0])) // comment?
			continue;
		char *first=buf, *last=buf, *end=buf+is.gcount()-1;
		MapEntry entry;
		while (last < end && *first) {
			last = first;
			switch (read_entry(first, last)) {
				case FM_NAME:
					cout  << "name: " << first << endl; break;
				case FM_ENC:
					cout  << "enc: " << first << endl; break;
				case FM_FONT:
					cout  << "font: " << first << endl; break;
				case FM_ERROR:
					continue;
				default:
					break;
			}
			first = last+1;
		}
		cout << "--------------------------------\n";
	}
}


void FontMap::readPdfMap (istream &is) {
	char buf[512];
	while (is && !is.eof()) {
		is.getline(buf, 512);
		if (!buf[0] || strchr(" %#;*", buf[0])) // comment?
			continue;
		char *first=buf, *last=buf, *end=buf+is.gcount()-1;
		vector<string> fields(3);
		for (int i=0; i < 3 && last < end && *first; i++) {
			FontMapFieldType type = read_entry(first, last, true);
			if (*first == '-')
				break;
			if (type == FM_NAME)
				fields[i] = first;
		}
		if (fields[1] == "default" || fields[1] == "none")
			fields[1] = "";
		if ((fields.size() == 2 && fields[1] == "") || (fields.size() == 3 && fields[1] == "" && fields[0] == fields[2]))
			continue;

		_fontMap[fields[0]].fontname = fields[fields.size() == 2 ? 0 : 2];
		_fontMap[fields[0]].encname  = fields[1];
	}
}


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


FontMap::FontMap (const char *fname) {
	if (fname) {
		ifstream ifs(fname);
		if (ifs) {
			if (strstr(fname, "dvipdfm"))
				readPdfMap(ifs);
			else
				readPsMap(ifs);
		}
	}
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
