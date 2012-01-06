/*************************************************************************
** FontMap.cpp                                                          **
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

#include <fstream>
#include <iostream>
#include <vector>
#include "Directory.h"
#include "FontMap.h"

using namespace std;

enum FontMapFieldType {FM_ERROR=0, FM_NONE, FM_NAME, FM_PS_CODE, FM_HEADER, FM_ENC, FM_FONT};


static char* str_tolower (char *str);
static inline char* get_extension (char *fname);
static FontMapFieldType read_entry (char* &first, char* &last, bool name_only=false);



FontMap::FontMap (const string &fname) {
	read(fname);
}


bool FontMap::read (const string &fname) {
	ifstream ifs(fname.c_str());
	if (ifs) {
		if (fname.find("dvipdfm") != string::npos)
			readPdfMap(ifs);
		else
			readPsMap(ifs);
		return true;
	}
	return false;
}


/** Read map file in dvips format.
 *  @param[in] is data is read from this stream */
void FontMap::readPsMap (istream &is) {
	char buf[512];
	while (is && !is.eof()) {
		is.getline(buf, 512);
		if (!buf[0] || strchr(" %#;*", buf[0])) // comment?
			continue;
		char *first=buf, *last=buf, *end=buf+is.gcount()-1;
		MapEntry entry;
		string name;
		while (last < end && *first) {
			last = first;
			switch (read_entry(first, last)) {
				case FM_NAME:
					if (name.empty())
						name = first;
					break;
				case FM_ENC:
					entry.encname = first; break;
				case FM_FONT:
					entry.fontname = first; break;
				case FM_ERROR:
					continue;
				default:
					break;
			}
			first = last+1;
		}
		// strip filename suffix
		size_t len;
		if ((len = entry.encname.length()) > 4 && entry.encname.substr(len-4) == ".enc")
			entry.encname = entry.encname.substr(0, len-4);
		if ((len = entry.fontname.length()) > 4 && entry.fontname[len-4] == '.')
			entry.fontname = entry.fontname.substr(0, len-4);

		if (!name.empty() && ((name != entry.fontname && !entry.fontname.empty()) || !entry.encname.empty()))
			_fontMap[name] = entry;
	}
}


/** Read map file in dvipdfm format.
 *  <font name> [<encoding>|default|none] [<map target>] [options]
 *  The optional trailing dvipdfm-parameters -r, -e and -s are ignored.
 *  @param[in] is data is read from this stream */
void FontMap::readPdfMap (istream &is) {
	char buf[512];
	while (is && !is.eof()) {
		is.getline(buf, 512);
		if (!buf[0] || strchr(" %#;*", buf[0])) // comment?
			continue;
		char *first=buf, *last=buf, *end=buf+is.gcount()-1;
		vector<string> fields;
		for (int i=0; i < 3 && last < end && *first; i++) {
			FontMapFieldType type = read_entry(first, last, true);
			if (*first == '-')
				break;
			if (type == FM_NAME)
				fields.push_back(first);
			first = last+1;
		}
		if (fields.size() > 1 && (fields[1] == "default" || fields[1] == "none"))
			fields[1].clear();

		if (fields.size() < 2)
			continue;
		if ((fields.size() == 2 && fields[1].empty()) || (fields.size() == 3 && fields[1].empty() && fields[0] == fields[2]))
			continue;

		_fontMap[fields[0]].fontname = fields[fields.size() == 2 ? 0 : 2];
		_fontMap[fields[0]].encname  = fields[1];
	}
}



static char* str_tolower (char *str) {
	for (char *p=str; *p; p++)
		*p = tolower(*p);
	return str;
}


static inline char* get_extension (char *fname) {
	if (char* p=strrchr(fname, '.'))
		return p+1;
	return 0;
}




/** Reads a single line entry.
 *  @param[in,out] first pointer to first char of entry
 *  @param[in,out] last  pointer to last char of entry
 *  @param[in] name_only true, if special meanings of \" and < should be ignored
 *  @return entry type */
static FontMapFieldType read_entry (char* &first, char* &last, bool name_only) {
	while (*first && isspace(*first))
		first++;
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
			FontMapFieldType type=FM_HEADER;
			first++;
			if (isspace(*first))
				first++;
			else if (*first == '<' || *first == '[') {
				type = (*first == '<') ? FM_FONT : FM_ENC;
				first++;
			}
			if (read_entry(first, last, true) != FM_NAME)
				return FM_ERROR;
			if (type == FM_HEADER) {
				if (char *ext = get_extension(first)) {
					ext = str_tolower(ext);
					if (strcmp(ext, "enc")==0)
						return FM_ENC;
					if (strcmp(ext, "pfb")==0 || strcmp(ext, "pfa")==0)
						return FM_FONT;
				}
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


ostream& FontMap::write (ostream &os) const {
	for (map<string,MapEntry>::const_iterator i=_fontMap.begin(); i != _fontMap.end(); ++i)
		os << i->first << " -> " << i->second.fontname << " [" << i->second.encname << "]\n";
	return os;
}


void FontMap::readdir (const string &dirname) {
	Directory dir(dirname);
	while (const char *fname = dir.read('f')) {
		if (strlen(fname) >= 4 && strcmp(fname+strlen(fname)-4, ".map") == 0) {
			string path = dirname + "/" + fname;
			read(path.c_str());
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
	if (it == _fontMap.end() || it->second.encname.empty())
		return 0;
	return it->second.encname.c_str();
}
