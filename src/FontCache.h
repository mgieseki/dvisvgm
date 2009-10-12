/*************************************************************************
** FontCache.h                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef FONTCACHE_H
#define FONTCACHE_H

#include <iostream>
#include <string>
#include <map>
#include "FontGlyph.h"

class FontCache
{
	typedef std::map<int, const Glyph*> GlyphMap;

	public:
		struct FontInfo
		{
			std::string name; // fontname
			UInt16 version;
			UInt32 numchars;  // number of characters
			UInt32 numbytes;  // number of bytes
			UInt32 numcmds;   // number of path commands
		};

   public:
		FontCache ();
      ~FontCache ();
		bool read (const char *fontname, const char *dir);
		bool read (const char *fontname, std::istream &is);
		bool write (const char *fontname, const char *dir) const;
		bool write (const char *fontname, std::ostream &os) const;
		const Glyph* getGlyph (int c) const;
		void setGlyph (int c, const Glyph *glyph);
		void clear ();

		static bool fontinfo (const char *dirname, std::vector<FontInfo> &infos);
		static bool fontinfo (std::istream &is, FontInfo &info);
		static void fontinfo (const char *dirname, ostream &os);

   private:
		const static UInt8 VERSION = 3;
		GlyphMap _glyphs;
		bool _changed;
};

#endif
