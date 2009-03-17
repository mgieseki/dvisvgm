/***********************************************************************
** FontCache.cpp                                                      **
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

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>
#include "FileSystem.h"
#include "FontCache.h"
#include "FontGlyph.h"
#include "types.h"

using namespace std;


static UInt32 read_unsigned (int bytes, istream &is) {
	UInt32 ret = 0;
	for (bytes--; bytes >= 0 && !is.eof(); bytes--) {
		UInt32 b = is.get();
		ret |= b << (8*bytes);
	}
	return ret;
}


static Int32 read_signed (int bytes, istream &is) {
	Int32 ret = is.get();
	if (ret & 128)        // negative value?
		ret |= 0xffffff00;
	for (bytes-=2; bytes >= 0 && !is.eof(); bytes--) 
		ret = (ret << 8) | is.get();
	return ret;
}


static void write_unsigned (UInt32 value, int bytes, ostream &os) {
	for (bytes--; bytes >= 0; bytes--)
		os.put((value >> (8*bytes)) & 0xff);
}


static inline void write_signed (Int32 value, int bytes, ostream &os) {
	if (value < 0)
		value |= 0x80 << (bytes-1);
	write_unsigned((UInt32)value, bytes, os);
}


static LPair read_pair (istream &is) {
	long x = read_signed(4, is);
	long y = read_signed(4, is);
	return LPair(x, y);
}


FontCache::FontCache () : _changed(false)
{
}


FontCache::~FontCache () {
	clear();
}


void FontCache::clear () {
	FORALL(_glyphs, GlyphMap::iterator, it)
		delete it->second;
	_glyphs.clear();
}


void FontCache::setGlyph (int c, const Glyph *glyph) {
	if (!glyph || glyph->empty())
		delete glyph;
	else {
		GlyphMap::iterator it = _glyphs.find(c);
		if (it != _glyphs.end()) {
			delete it->second;
			it->second = glyph;
		}
		else
			_glyphs[c] = glyph;
		_changed = true;
	}
}


const Glyph* FontCache::getGlyph (int c) const {
	GlyphMap::const_iterator it = _glyphs.find(c);
	return (it != _glyphs.end()) ? it->second : 0;
}


bool FontCache::write (const char *fontname, const char *dir) const {
	if (!_changed)
		return true;

	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
		ofstream ofs(oss.str().c_str(), ios::binary);
		return write(fontname, ofs);
	}
	return false;
}


bool FontCache::write (const char *fontname, ostream &os) const {
	if (!_changed)
		return true;

	if (os) {
		write_unsigned(VERSION, 1, os);
		for (const char *p=fontname; *p; p++)
			os.put(*p);
		os.put(0);
		write_unsigned(_glyphs.size(), 4, os);
		FORALL(_glyphs, GlyphMap::const_iterator, it) {
			write_unsigned(it->first, 4, os);
			write_unsigned(it->second->commands().size(), 2, os);
			FORALL(it->second->commands(), list<GlyphCommand*>::const_iterator, cit) {
				write_unsigned((*cit)->getSVGPathCommand(), 1, os);
				const vector<LPair> &params = (*cit)->params();
				for (size_t i=0; i < params.size(); i++) {
					write_signed(params[i].x(), 4, os);
					write_signed(params[i].y(), 4, os);
				}
			}
		}
		return true;
	}
	return false;
}


bool FontCache::read (const char *fontname, const char *dir) {
	clear();
	if (fontname && strlen(fontname) > 0) {
		if (dir == 0 || strlen(dir) == 0)
			dir = FileSystem::getcwd().c_str();
		ostringstream oss;
		oss << dir << '/' << fontname << ".fgd";
		ifstream ifs(oss.str().c_str(), ios::binary);
		return read(fontname, ifs);
	}
	return false;
}


bool FontCache::read (const char *fontname, istream &is) {
	clear();
	if (is) {
		if (read_unsigned(1, is) != VERSION)
			return false;
		string fname;		
		while (!is.eof() && is.peek() != 0)
			fname += is.get();
		is.get(); // skip 0-byte
		UInt32 num_glyphs = read_unsigned(4, is);
		while (num_glyphs-- > 0) {
			UInt32 c = read_unsigned(4, is);
			UInt16 s = read_unsigned(2, is);
			Glyph *glyph = new Glyph;
			while (s-- > 0) {
				UInt8 cmdchar = read_unsigned(1, is);
				GlyphCommand *cmd=0;
				switch (cmdchar) {
					case 'C': {
						LPair p1 = read_pair(is);
						LPair p2 = read_pair(is);
						LPair p3 = read_pair(is);
						cmd = new GlyphCubicTo(p1, p2, p3);					
						break;
					}
					case 'L':
						cmd = new GlyphLineTo(read_pair(is));
						break;
					case 'M': 
						cmd = new GlyphMoveTo(read_pair(is));
						break;
					case 'Q': {
						LPair p1 = read_pair(is);
						LPair p2 = read_pair(is);
						cmd = new GlyphConicTo(p1, p2);
						break;
					}
					case 'S': {
						LPair p1 = read_pair(is);
						LPair p2 = read_pair(is);
						cmd = new GlyphShortCubicTo(p1, p2);
						break;
					}
					case 'T':
						cmd = new GlyphShortConicTo(read_pair(is));
						break;
					case 'Z':
						cmd = new GlyphClosePath();
				}
				if (cmd)
					glyph->addCommand(cmd);
			}
			setGlyph(c, glyph);
		}
		_changed = false;
		return true;
	}
	return false;
}
