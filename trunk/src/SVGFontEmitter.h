/***********************************************************************
** SVGFontEmitter.h                                                   **
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

#ifndef SVGFONTDEFEMITTER_H
#define SVGFONTDEFEMITTER_H

#include <string>
#include "FontEmitter.h"
#include "FontEngine.h"

using std::set;
using std::string;

class CharmapTranslator;
class XMLElementNode;

class SVGFontEmitter : public FontEmitter
{
   public:
      SVGFontEmitter (const string &fname, const CharmapTranslator &cmt, XMLElementNode *n);
		void readFontFile (const string &fname);
		int emitFont (string id="") const;
		int emitFont (const set<int> &usedChars, string id="") const;
		bool emitGlyph (int c) const;
		const XMLElementNode* getGlyphNode () const {return glyphNode;}
		
	protected:
		int emitFont (const set<int> *usedCharsm, string id) const;

   private:
		FontEngine fontEngine;
		const CharmapTranslator &charmapTranslator;
		XMLElementNode *rootNode;          // 
		mutable XMLElementNode *glyphNode; // current <glyph ...>-node
};

#endif
