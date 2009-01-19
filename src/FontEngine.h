/***********************************************************************
** FontEngine.h                                                       **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef FONTENGINE_H
#define FONTENGINE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <vector>
#include "types.h"

using std::map;
using std::string;
using std::vector;


struct FEGlyphCommands
{
	virtual ~FEGlyphCommands () {}
	virtual void moveTo(long x, long y) {}
	virtual void lineTo(long x, long y) {}
	virtual void conicTo(long x1, long y1, long x2, long y2) {}
	virtual void cubicTo(long x1, long y1, long x2, long y2, long x3, long y3) {}
};


/** This class provides methods to handle font files and font data. 
 *  It's a wrapper for the Freetype font library. */
class FontEngine
{
   public:
      FontEngine ();
      ~FontEngine ();
		void setDeviceResolution (int x, int y);
      bool setFont (const string &fname, int ptSize=0);
		bool setCharSize (int ptSize);
		bool getGlyphMetrics (unsigned char chr, int &width, int &height, int &depth) const;
		bool traceOutline (unsigned char chr, FEGlyphCommands &commands, bool scale=true) const;
		const char* getFamilyName () const;
		const char* getStyleName () const;
		int getUnitsPerEM () const;
		int getAscender () const;
		int getDescender () const;
		int getHAdvance () const;
		int getHAdvance (unsigned int c) const;
//		int getVAdvance () const;
		int getFirstChar () const;
		int getNextChar () const;
		vector<int> getPanose () const;
		string getGlyphName (unsigned int c) const;
		int getCharByGlyphName (const char *name) const;
		void buildTranslationMap (map<UInt32,UInt32> &translationMap) const;

   private:
		int horDeviceRes, vertDeviceRes;
		mutable unsigned int currentChar, currentGlyphIndex;
		FT_Face currentFace;
      FT_Library library;
};

#endif
