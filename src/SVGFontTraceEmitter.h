/***********************************************************************
** SVGFontTraceEmitter.h                                              **
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

#ifndef SVGFONTTRACEREMITTER_H
#define SVGFONTTRACEREMITTER_H

#include <istream>
#include <set>
#include "CharmapTranslator.h"
#include "FontEmitter.h"
#include "XMLNode.h"


class FileFinder;
class Font;
class FontManager;
class GFGlyphTracer;
class XMLElementNode;

class SVGFontTraceEmitter : public FontEmitter
{
   public:
      SVGFontTraceEmitter (const Font *font, const FontManager &fm, const CharmapTranslator &cmt, XMLElementNode *n, bool uf);
      ~SVGFontTraceEmitter ();
		int emitFont (const char *id) const;
		int emitFont (const std::set<int> &usedChars, const char *id) const;
		bool emitGlyph (int c) const;
		void setMag (double m) {_mag = m;}

	protected:
		int emitFont (const std::set<int> *usedChars, const char *id) const;
		bool checkTracer () const;

   private:
		mutable GFGlyphTracer *_gfTracer;
		mutable std::istream *_in;
		const Font *_font;
		const FontManager &_fontManager;
		double _mag;
		const CharmapTranslator &_charmapTranslator;
		XMLElementNode *_rootNode; 
		mutable XMLElementNode *_glyphNode;
		bool _useFonts;
};

#endif
