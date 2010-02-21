/*************************************************************************
** SVGFontEmitter.cpp                                                   **
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


#include <sstream>
#include "macros.h"
#include "CharmapTranslator.h"
#include "Font.h"
#include "FontEncoding.h"
#include "FontEngine.h"
#include "FontManager.h"
#include "Glyph.h"
#include "SVGFontEmitter.h"
#include "SVGTree.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


SVGFontEmitter::SVGFontEmitter (const Font *font, const FontManager &fm, const CharmapTranslator &cmt, SVGTree &svg, bool uf)
	: _fontManager(fm), _charmapTranslator(cmt), _svg(svg), _useFonts(uf)
{
	_font = font;
	FontEngine::instance().setFont(*font);
}



int SVGFontEmitter::emitFont (const char *id) {
	return emitFont(0, id);
}


int SVGFontEmitter::emitFont (const set<int> &usedChars, const char *id) {
	return emitFont(&usedChars, id);
}


int SVGFontEmitter::emitFont (const set<int> *usedChars, const char *id) {
	if (!usedChars || usedChars->empty())
		return 0;

	XMLElementNode *fontNode=0;
	if (_useFonts) {
		FontEngine &fe = FontEngine::instance();
		fontNode = new XMLElementNode("font");
		if (id && strlen(id) > 0)
			fontNode->addAttribute("id", id);
		fontNode->addAttribute("horiz-adv", XMLString(fe.getHAdvance()));
		_svg.appendToDefs(fontNode);

		XMLElementNode *faceNode = new XMLElementNode("font-face");
		faceNode->addAttribute("font-family", (id && strlen(id) > 0) ? id : fe.getFamilyName());
		faceNode->addAttribute("units-per-em", XMLString(fe.getUnitsPerEM()));
		faceNode->addAttribute("ascent", XMLString(fe.getAscender()));
		faceNode->addAttribute("descent", XMLString(fe.getDescender()));
		fontNode->append(faceNode);
		FORALL(*usedChars, set<int>::const_iterator, i) {
			emitGlyph(*i);  // create new glyphNode
			fontNode->append(_glyphNode);
		}
	}
	else {
		FORALL(*usedChars, set<int>::const_iterator, i) {
			emitGlyph(*i);  // create new glyphNode
			_svg.appendToDefs(_glyphNode);
		}
	}
	return usedChars->size();
}


bool SVGFontEmitter::emitGlyph (int c) {
	double sx=1.0, sy=1.0;
	FontEncoding *encoding = _font->encoding();
	FontEngine &fe = FontEngine::instance();
	if (_useFonts) {
		_glyphNode = new XMLElementNode("glyph");
		_glyphNode->addAttribute("unicode", XMLString(_charmapTranslator.unicode(c), false));
		int advance;
		string name;
		if (encoding && encoding->getEntry(c)) {
			advance = fe.getHAdvance(encoding->getEntry(c));
			name = encoding->getEntry(c);
		}
		else {
			advance = fe.getHAdvance(c);
			name = fe.getGlyphName(c);
		}
		_glyphNode->addAttribute("horiz-adv-x", XMLString(advance));
		_glyphNode->addAttribute("glyph-name", name);
	}
	else {
		ostringstream oss;
		oss << 'g' << _fontManager.fontID(_font) << c;
		_glyphNode = new XMLElementNode("path");
		_glyphNode->addAttribute("id" , oss.str());
		sx = double(_font->scaledSize())/fe.getUnitsPerEM();
		sy = -sx;
	}
	ostringstream path;
	Glyph glyph;
	_font->getGlyph(c, glyph);
//	glyph.read(c, encoding, _fontEngine);
	glyph.closeOpenSubPaths();
	glyph.writeSVG(path, sx, sy);
	_glyphNode->addAttribute("d", path.str());
	return true;
}

