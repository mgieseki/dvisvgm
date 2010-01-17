/*************************************************************************
** FontEngine.h                                                         **
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

#ifndef FONTENGINE_H
#define FONTENGINE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <vector>
#include "Glyph.h"
#include "types.h"


/** This class provides methods to handle font files and font data.
 *  It's a wrapper for the Freetype font library. */
class FontEngine
{
   public:
      FontEngine ();
      ~FontEngine ();
		void setDeviceResolution (int x, int y);
      bool setFont (const std::string &fname, int ptSize=0);
		bool setCharSize (int ptSize);
		bool traceOutline (unsigned char chr, Glyph &glyph, bool scale=true) const;
		bool traceOutline (const char *name, Glyph &glyph, bool scale) const;
		const char* getFamilyName () const;
		const char* getStyleName () const;
		int getUnitsPerEM () const;
		int getAscender () const;
		int getDescender () const;
		int getHAdvance () const;
		int getHAdvance (unsigned int c) const;
		int getHAdvance (const char *name) const;
		int getFirstChar () const;
		int getNextChar () const;
		int getCharSize () const {return _ptSize;}
		std::vector<int> getPanose () const;
		std::string getGlyphName (unsigned int c) const;
		int getCharByGlyphName (const char *name) const;
		void buildTranslationMap (std::map<UInt32,UInt32> &translationMap) const;

   private:
		int _horDeviceRes, _vertDeviceRes;
		mutable unsigned int _currentChar, _currentGlyphIndex;
		FT_Face             _currentFace;
      FT_Library          _library;
		int                 _ptSize;
};

#endif
