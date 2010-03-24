/*************************************************************************
** FontCache.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "FileSystem.h"
#include "FontCache.h"
#include "Glyph.h"
#include "Pair.h"
#include "StreamReader.h"
#include "StreamWriter.h"
//#include "gzstream.h"
#include "types.h"

using namespace std;


static Pair32 read_pair (int bytes, StreamReader &sr) {
	Int32 x = sr.readSigned(bytes);
	Int32 y = sr.readSigned(bytes);
	return Pair32(x, y);
}


FontCache::FontCache () : _changed(false)
{
}


FontCache::~FontCache () {
	clear();
}


/** Removes all data from the cache. This does not affect the cache files. */
void FontCache::clear () {
	_glyphs.clear();
	_fontname.clear();
}


/** Assigns glyph data to a character and adds it to the cache.
 *  @param[in] c character code
 *  @param[in] glyph font glyph data */
void FontCache::setGlyph (int c, const Glyph &glyph) {
	if (!glyph.empty()) {
		_glyphs[c] = glyph;
		_changed = true;
	}
}


/** Returns the corresponding glyph data of a given character of the current font.
 *  @param[in] c character code
 *  @return font glyph data (0 if no matching data was found) */
const Glyph* FontCache::getGlyph (int c) const {
	GlyphMap::const_iterator it = _glyphs.find(c);
	return (it != _glyphs.end()) ? &it->second : 0;
}


/** Writes the current cache data to a file (only if anything changed after
 *  the last call of read()).
 *  @param[in] fontname name of current font
 *  @param[in] dir directory where the cache file should go
 *  @return true if writing was successful */
bool FontCache::write (const char *fontname, const char *dir) const {
	if (!_changed)
		return true;

	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
//		ogzstream ofs(oss.str().c_str(), 9, ios::binary|ios::out);
		ofstream ofs(oss.str().c_str(), ios::binary);
		return write(fontname, ofs);
	}
	return false;
}


bool FontCache::write (const char* dir) const {
	return _fontname.empty() ? false : write(_fontname.c_str(), dir);
}


/** Returns the minimal number of bytes needed to store the given value. */
static int max_int_size (Int32 value) {
	Int32 limit = 0x7f;
	for (int i=1; i <= 4; i++) {
		if ((value < 0  && -value <= limit+1) || (value >= 0 && value <= limit))
			return i;
		limit = (limit << 8) | 0xff;
	}
	return 4;
}


/** Returns the minimal number of bytes needed to store the biggest
 *  pair component of the given vector. */
static int max_int_size (const Pair<Int32> *pairs, size_t n) {
	int ret=0;
	for (size_t i=0; i < n; i++) {
		ret = max(ret, max_int_size(pairs[i].x()));
		ret = max(ret, max_int_size(pairs[i].y()));
	}
	return ret;
}


/** Writes the current cache data to a stream (only if anything changed after
 *  the last call of read()).
 *  @param[in] fontname name of current font
 *  @param[in] os output stream
 *  @return true if writing was successful */
bool FontCache::write (const char *fontname, ostream &os) const {
	if (!_changed)
		return true;
	if (!os)
		return false;

	StreamWriter sw(os);

	struct WriteActions : Glyph::Actions {
		WriteActions (StreamWriter &sw) : _sw(sw) {}

		void draw (char cmd, const Glyph::Point *points, int n) {
			int bytes = max_int_size(points, n);
			UInt8 cmdchar = (bytes << 5) | (cmd - 'A');
			_sw.writeUnsigned(cmdchar, 1);
			for (int i=0; i < n; i++) {
				_sw.writeSigned(points[i].x(), bytes);
				_sw.writeSigned(points[i].y(), bytes);
			}
		}
		StreamWriter &_sw;
	} actions(sw);

	sw.writeUnsigned(VERSION, 1);
	sw.writeString(fontname, true);
	sw.writeUnsigned(_glyphs.size(), 4);
	FORALL(_glyphs, GlyphMap::const_iterator, it) {
		const Glyph &glyph = it->second;
		sw.writeUnsigned(it->first, 4);
		sw.writeUnsigned(glyph.size(), 2);
		glyph.iterate(actions, false);
	}
	return true;
}


/** Reads font glyph information from a file.
 *  @param[in] fontname name of font data to read
 *  @param[in] dir directory where the cache files are located
 *  @return true if reading was successful */
bool FontCache::read (const char *fontname, const char *dir) {
	if (_fontname == fontname)
		return true;
	clear();
	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
		ifstream ifs(oss.str().c_str(), ios::binary);
//		igzstream ifs(oss.str().c_str(), 9, ios::binary|ios::in);
		return read(fontname, ifs);
	}
	return false;
}


/** Reads font glyph information from a stream.
 *  @param[in] fontname name of font data to read
 *  @param[in] dir input stream
 *  @return true if reading was successful */
bool FontCache::read (const char *fontname, istream &is) {
	if (_fontname == fontname)
		return true;
	if (!is)
		return false;

	clear();
	_fontname = fontname;

	StreamReader sr(is);
	if (sr.readUnsigned(1) != VERSION)
		return false;
	string fname;
	while (!is.eof() && is.peek() != 0)
		fname += is.get();
	is.get(); // skip 0-byte
	if (fname != fontname)
		return false;
	UInt32 num_glyphs = sr.readUnsigned(4);
	while (num_glyphs-- > 0) {
		UInt32 c = sr.readUnsigned(4);  // character code
		UInt16 s = sr.readUnsigned(2);  // number of path commands
		Glyph &glyph = _glyphs[c];
		while (s-- > 0) {
			UInt8 cmdval = sr.readUnsigned(1);
			UInt8 cmdchar = (cmdval & 0x1f) + 'A';
			int bytes = cmdval >> 5;
			switch (cmdchar) {
				case 'C': {
					Pair32 p1 = read_pair(bytes, sr);
					Pair32 p2 = read_pair(bytes, sr);
					Pair32 p3 = read_pair(bytes, sr);
					glyph.cubicto(p1, p2, p3);
					break;
				}
				case 'L':
					glyph.lineto(read_pair(bytes, sr));
					break;
				case 'M':
					glyph.moveto(read_pair(bytes, sr));
					break;
				case 'Q': {
					Pair32 p1 = read_pair(bytes, sr);
					Pair32 p2 = read_pair(bytes, sr);
					glyph.conicto(p1, p2);
					break;
				}
				case 'Z':
					glyph.closepath();
			}
		}
	}
	_changed = false;
	return true;
}


/** Collects font cache information.
 *  @param[in]  dirname path to font cache directory
 *  @param[out] infos the collected data
 *  @return true on success */
bool FontCache::fontinfo (const char *dirname, std::vector<FontInfo> &infos) {
	infos.clear();
	if (dirname) {
		vector<string> fnames;
		FileSystem::collect(dirname, fnames);
		FORALL(fnames, vector<string>::iterator, it) {
			if ((*it)[0] == 'f' && it->length() > 5 && it->substr(it->length()-4) == ".fgd") {
				FontInfo info;
				string path = string(dirname)+"/"+(it->substr(1));
				ifstream ifs(path.c_str(), ios::binary);
				if (fontinfo(ifs, info))
					infos.push_back(info);
			}
		}
	}
	return infos.size() > 0;
}


/** Collects font cache information of a single font.
 *  @param[in]  is input stream of the cache file
 *  @param[out] info the collected data
 *  @return true on success */
bool FontCache::fontinfo (std::istream &is, FontInfo &info) {
	info.name.clear();
	info.numchars = info.numbytes = info.numcmds = 0;
	if (is) {
		StreamReader sr(is);
		if ((info.version = sr.readUnsigned(1)) != VERSION)
			return false;
		while (!is.eof() && is.peek() != 0)
			info.name += is.get();
		is.get(); // skip 0-byte
		info.numchars = sr.readUnsigned(4);
		for (UInt32 i=0; i < info.numchars; i++) {
			sr.readUnsigned(4);  // character code
			UInt16 s = sr.readUnsigned(2);  // number of path commands
			while (s-- > 0) {
				UInt8 cmdval = sr.readUnsigned(1);
				UInt8 cmdchar = (cmdval & 0x1f) + 'A';
				int bytes = cmdval >> 5;
				int bc = 0;
				switch (cmdchar) {
					case 'C': bc = 6*bytes; break;
					case 'H':
					case 'L':
					case 'M':
					case 'T':
					case 'V': bc = 2*bytes; break;
					case 'Q':
					case 'S': bc = 4*bytes; break;
					case 'Z': break;
					default : return false;
				}
				info.numbytes += bc+1; // command length + command
				info.numcmds++;
				is.seekg(bc, ios_base::cur);
			}
			info.numbytes += 6; // number of path commands + char code
		}
		info.numbytes += 6+info.name.length(); // version + 0-byte + fontname + number of chars
	}
	return true;
}


/** Collects font cache information and write it to a stream.
 *  @param[in] dirname path to font cache directory
 *  @param[in] os output is written to this stream */
void FontCache::fontinfo (const char *dirname, ostream &os) {
	vector<FontInfo> infos;
	if (fontinfo(dirname, infos)) {
		os << "cache format version " << infos[0].version << endl;
		typedef map<string,FontInfo*> SortMap;
		SortMap sortmap;
		FORALL(infos, vector<FontInfo>::iterator, it)
			sortmap[it->name] = &(*it);

		FORALL(sortmap, SortMap::iterator, it) {
			os << setw(10) << left  << it->second->name
				<< setw(5)  << right << it->second->numchars << " chars"
				<< setw(10) << right << it->second->numcmds  << " cmds"
				<< setw(10) << right << it->second->numbytes << " bytes"
				<< endl;
		}
	}
	else
		os << "cache is empty\n";
}
