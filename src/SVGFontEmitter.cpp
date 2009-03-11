/***********************************************************************
** SVGFontEmitter.cpp                                                 **
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

#include <sstream>
#include "macros.h"
#include "CharmapTranslator.h"
#include "Font.h"
#include "FontEncoding.h"
#include "FontEngine.h"
#include "FontManager.h"
#include "FontGlyph.h"
#include "SVGFontEmitter.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;

SVGFontEmitter::SVGFontEmitter (const Font *font, const FontManager &fm, const CharmapTranslator &cmt, XMLElementNode *root, bool uf) 
	: _fontManager(fm), _charmapTranslator(cmt), _rootNode(root), _useFonts(uf)
{
	_font = font;
	_fontEngine.setFont(font->path());
}



int SVGFontEmitter::emitFont (const char *id) const {
	return emitFont(0, id);
}


int SVGFontEmitter::emitFont (const set<int> &usedChars, const char *id) const {
	return emitFont(&usedChars, id);
}


int SVGFontEmitter::emitFont (const set<int> *usedChars, const char *id) const {
	if (!usedChars || usedChars->empty())
		return 0;

	XMLElementNode *fontNode=0;
	if (_useFonts) {
		fontNode = new XMLElementNode("font");
		if (id && strlen(id) > 0)
			fontNode->addAttribute("id", id);
		fontNode->addAttribute("horiz-adv", XMLString(_fontEngine.getHAdvance()));
		_rootNode->append(fontNode);

		XMLElementNode *faceNode = new XMLElementNode("font-face");
		faceNode->addAttribute("font-family", (id && strlen(id) > 0) ? id : _fontEngine.getFamilyName());
		faceNode->addAttribute("units-per-em", XMLString(_fontEngine.getUnitsPerEM()));
		faceNode->addAttribute("ascent", XMLString(_fontEngine.getAscender()));
		faceNode->addAttribute("descent", XMLString(_fontEngine.getDescender()));
		fontNode->append(faceNode);
	}
	else {
		fontNode = _rootNode;
#if 0
		if (usedChars && _font && !usedChars->empty()) {
			ostringstream oss;
			oss << _font->name() << ", " << _font->scaledSize() << "pt"; 
			fontNode->append(new XMLCommentNode(oss.str()));
		}
#endif
	}
	FORALL(*usedChars, set<int>::const_iterator, i) {
		emitGlyph(*i);  // create new glyphNode
		fontNode->append(_glyphNode);
	}
	return usedChars->size();
}


bool SVGFontEmitter::emitGlyph (int c) const {
	double sx=1.0, sy=1.0;
	FontEncoding *encoding = _fontManager.encoding(_font);
	if (_useFonts) {
		_glyphNode = new XMLElementNode("glyph");
		_glyphNode->addAttribute("unicode", XMLString(_charmapTranslator.unicode(c), false));
		int advance;
		string name;
		if (encoding && encoding->getEntry(c)) {
			advance = _fontEngine.getHAdvance(encoding->getEntry(c));
			name = encoding->getEntry(c);
		}
		else {
			advance = _fontEngine.getHAdvance(c);
			name = _fontEngine.getGlyphName(c);
		}
		_glyphNode->addAttribute("horiz-adv-x", XMLString(advance));
		_glyphNode->addAttribute("glyph-name", name);
	}
	else {
		ostringstream oss;
		oss << 'g' << _fontManager.fontID(_font) << c;
		_glyphNode = new XMLElementNode("path");
		_glyphNode->addAttribute("id" , oss.str());
		sx = double(_font->scaledSize())/_fontEngine.getUnitsPerEM();
		sy = -sx;
	}
	ostringstream path;
	Glyph glyph;
	glyph.read(c, encoding, _fontEngine);
	glyph.closeOpenPaths();
//	glyph.optimizeCommands();
	glyph.writeSVGCommands(path, sx, sy);
	_glyphNode->addAttribute("d", path.str());
	return true;
}

