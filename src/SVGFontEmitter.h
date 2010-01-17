/*************************************************************************
** SVGFontEmitter.h                                                     **
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

#ifndef SVGFONTDEFEMITTER_H
#define SVGFONTDEFEMITTER_H


#include "FontEmitter.h"
#include "FontEngine.h"


class CharmapTranslator;
class FontManager;
class SVGTree;
class XMLElementNode;

class SVGFontEmitter : public FontEmitter
{
   public:
      SVGFontEmitter (const Font *font, const FontManager &fm, const CharmapTranslator &cmt, SVGTree &svg, bool uf);
		int emitFont (const char *id);
		int emitFont (const std::set<int> &usedChars, const char *id);
		bool emitGlyph (int c);
		const XMLElementNode* getGlyphNode () const {return _glyphNode;}

	protected:
		int emitFont (const std::set<int> *usedCharsm, const char *id);

   private:
		const Font *_font;
		FontEngine _fontEngine;
		const FontManager &_fontManager;
		const CharmapTranslator &_charmapTranslator;
		SVGTree &_svg;
		mutable XMLElementNode *_glyphNode; // current <glyph ...>-node
		bool _useFonts;  ///< create font elements or draw paths?
};

#endif
