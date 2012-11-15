/*************************************************************************
** FontMap.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <limits>
#include <vector>
#include "Directory.h"
#include "FontMap.h"
#include "MapLine.h"
#include "Message.h"

using namespace std;


/** Returns the singleton instance. */
FontMap& FontMap::instance() {
	static FontMap fontmap;
	return fontmap;
}


/** Reads and evaluates a font map file.
 *  @param[in] fname name of mapfile to read
 *  @return true if file could be opened */
bool FontMap::read (const string &fname, FontMap::Mode mode) {
	ifstream ifs(fname.c_str());
   if (!ifs)
      return false;

	int line_number = 1;
	while (ifs) {
		if (strchr("\n&#%;*", ifs.peek()))  // comment line?
			ifs.ignore(numeric_limits<int>::max(), '\n');
		else {
			try {
				MapLine mapline(ifs);
				apply(mapline, mode);
			}
			catch (const MapLineException &ex) {
				Message::wstream(true) << "map file " << fname << ", line " << line_number << ": " << ex.what() << '\n';
			}
		}
      line_number++;
	}
	return true;
}


bool FontMap::read (const string &fname, char modechar) {
	Mode mode;
	switch (modechar) {
		case '=': mode = FM_REPLACE; break;
		case '-': mode = FM_REMOVE; break;
		default : mode = FM_APPEND;
	}
	return read(fname, mode);
}


/** Applies a mapline according to the given mode (append, remove, replace).
 *  @param[in] mapline the mapline to be applied
 *  @param[in] mode mode to use
 *  @return true in case of success */
bool FontMap::apply (const MapLine& mapline, FontMap::Mode mode) {
	switch (mode) {
		case FM_APPEND:
			return append(mapline);
		case FM_REMOVE:
			return remove(mapline);
		default:
			return replace(mapline);
	}
}


/** Applies a mapline according to the given mode (append, remove, replace).
 *  @param[in] mapline the mapline to be applied
 *  @param[in] modechar character that denotes the mode (+, -, or =)
 *  @return true in case of success */
bool FontMap::apply (const MapLine& mapline, char modechar) {
	Mode mode;
	switch (modechar) {
		case '=': mode = FM_REPLACE; break;
		case '-': mode = FM_REMOVE; break;
		default : mode = FM_APPEND;
	}
	return apply(mapline, mode);
}


/** Appends given mapline data to the fontmap if there is no entry for the corresponding font in the map yet.
 *  @param[in] mapline parsed font data
 *  @return true if data has been appended */
bool FontMap::append (const MapLine &mapline) {
   if (!mapline.texname().empty()) {
		if (!mapline.fontfname().empty() || !mapline.encname().empty()) {
			Iterator it = _fontMap.find(mapline.texname());
			if (it == _fontMap.end()) {
				_fontMap[mapline.texname()] = MapEntry(mapline.fontfname(), mapline.encname());
				return true;
			}
		}
	}
	return false;
}


/** Replaces the map data of the given font. If the font is locked (because it's already in use) nothing happens.
 *  @param[in] mapline parsed font data
 *  @return true if data has been replaced */
bool FontMap::replace (const MapLine &mapline) {
   if (!mapline.texname().empty()) {
		if (mapline.fontfname().empty() && mapline.encname().empty())
			return remove(mapline);
		Iterator it = _fontMap.find(mapline.texname());
		if (it == _fontMap.end())
			_fontMap[mapline.texname()] = MapEntry(mapline.fontfname(), mapline.encname());
		else if (!it->second.locked)
			it->second = MapEntry(mapline.fontfname(), mapline.encname());
		return true;
	}
	return false;
}


/** Removes the map entry of the given font. If the font is locked (because it's already in use) nothing happens.
 *  @param[in] mapline parsed font data
 *  @return true if entry has been removed */
bool FontMap::remove (const MapLine &mapline) {
   if (!mapline.texname().empty()) {
      Iterator it = _fontMap.find(mapline.texname());
      if (it != _fontMap.end() && !it->second.locked) {
         _fontMap.erase(it);
         return true;
      }
   }
   return false;
}


ostream& FontMap::write (ostream &os) const {
	for (map<string,MapEntry>::const_iterator i=_fontMap.begin(); i != _fontMap.end(); ++i)
		os << i->first << " -> " << i->second.fontname << " [" << i->second.encname << "]\n";
	return os;
}


/** Reads and evaluates all map files in the given directory.
 *  @param[in] path to directory containing the map files to be read */
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


/** Sets the lock flag for the given font in order to avoid changing the map data of this font.
 *  @param[in] fontname name of font to be locked */
void FontMap::lockFont (const string& fontname) {
	Iterator it = _fontMap.find(fontname);
	if (it != _fontMap.end())
		it->second.locked = true;
}
