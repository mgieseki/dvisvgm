/*************************************************************************
** DVIToSVGActions.cpp                                                  **
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstring>
#include "BoundingBox.h"
#include "CharmapTranslator.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "FontManager.h"
#include "Ghostscript.h"
#include "SpecialManager.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


unsigned DVIToSVGActions::PROGRESSBAR = 0;


DVIToSVGActions::DVIToSVGActions (DVIToSVG &dvisvg, SVGTree &svg) 
	: _svg(svg), _dvisvg(dvisvg), _pageMatrix(0), _bgcolor(Color::WHITE)
{
	_currentFontNum = -1;
	_pageCount = 0;
}


DVIToSVGActions::~DVIToSVGActions () {
	delete _pageMatrix;
	FORALL (_charmapTranslatorMap, CharmapTranslatorMap::iterator, i)
		delete i->second;
}


void DVIToSVGActions::setPageMatrix (const Matrix &matrix) {
	delete _pageMatrix;
	_pageMatrix = new Matrix(matrix);
}


/** This method is called when a "set char" command was found in the DVI file. 
 *  It draws a character of the current font.
 *  @param[in] x horizontal position of left bounding box edge 
 *  @param[in] y vertical position of the character's baseline 
 *  @param[in] c character code relative to the current font 
 *  @param[in] font font to be used */
void DVIToSVGActions::setChar (double x, double y, unsigned c, const Font *font) {
/*	x *= BP;
	y *= BP; */
	if (DVIToSVG::USE_FONTS) {
		// If we use SVG fonts there is no need to record all font name/char/size combinations
		// because the SVG font mechanism handles this automatically. It's sufficient to
		// record font names and chars. The various font sizes can be ignored here.
		// For a given font object, Font::uniqueFont() returns the same unique font object for
		// all fonts with the same name.
		font = font->uniqueFont();
	}
	_usedCharsMap[font].insert(c);

	const CharmapTranslator *cmt = _charmapTranslatorMap[font->uniqueFont()];
	_svg.appendChar(c, x, y, _dvisvg.getFontManager(), *cmt);

	// update bounding box
	if (font) {
		/*	x *= BP;
			y *= BP;*/
		double s = font->scaleFactor(); // * BP;
		double w = s*font->charWidth(c);
		double h = s*font->charHeight(c);
		double d = s*font->charDepth(c);
		BoundingBox charbox(x, y-h, x+w, y+d);
		if (!getMatrix().isIdentity())
			charbox.transform(getMatrix());
		_bbox.embed(charbox);
	}
}


/** This method is called when a "set rule" or "put rule" command was found in the 
 *  DVI file. It draws a solid unrotated rectangle.
 *  @param[in] x horizontal position of left edge 
 *  @param[in] y vertical position of bottom(!) edge 
 *  @param[in] height length of the vertical edges 
 *  @param[in] width length of the horizontal edges */
void DVIToSVGActions::setRule (double x, double y, double height, double width) {
/*	x *= BP;
	y *= BP;
	height *= BP;
	width  *= BP; */
	// (x,y) is the lower left corner of the rectangle
	XMLElementNode *rect = new XMLElementNode("rect");
	rect->addAttribute("x", x);
	rect->addAttribute("y", y-height);
	rect->addAttribute("height", height);
	rect->addAttribute("width", width);
	if (!getMatrix().isIdentity())
		rect->addAttribute("transform", getMatrix().getSVG());
	if (getColor() != Color::BLACK)
		rect->addAttribute("fill", _svg.getColor().rgbString());
	_svg.appendToPage(rect);
	
	// update bounding box
	BoundingBox bb(x, y+height, x+width, y);
	if (!getMatrix().isIdentity())
		bb.transform(getMatrix());
	_bbox.embed(bb);
}


void DVIToSVGActions::defineFont (int num, const Font *font) {
	font = font->uniqueFont();
	if (_charmapTranslatorMap.find(font) == _charmapTranslatorMap.end())
		_charmapTranslatorMap[font] = new CharmapTranslator(font);
}


/** This method is called when a "set font" command was found in the DVI file. The
 *  font must be previously defined.
 *  @param[in] num unique number of the font in the DVI file (not necessarily equal to the DVI font number)
 *  @param[in] font pointer to the font object (always represents a physical font and never a virtual font) */
void DVIToSVGActions::setFont (int num, const Font *font) {
	_currentFontNum = num;
	_svg.setFont(num, font);
}


/** This method is called when a "special" command was found in the DVI file. 
 *  @param[in] s the special expression */
void DVIToSVGActions::special (const string &s) {
	try {
		_dvisvg.specialManager().process(s, this, this);
		// @@ output message in case of unsupported specials?
	}
	catch (const SpecialException &e) {
		Message::estream(true) << "error in special '" << s << "': " << e.getMessage() << endl;
	}
}


void DVIToSVGActions::beginSpecial (const char *prefix) {
	static unsigned count = 0;
	if (PROGRESSBAR > 0 && ++count % PROGRESSBAR == 0) {
		if (count == PROGRESSBAR)
			Message::mstream(false) << ':';
		Message::mstream(false) << '=';
	}
}


void DVIToSVGActions::endSpecial (const char *) {
}


/** This method is called when the DVI preamble was read 
 *  @param[in] cmt preamble comment text. */
void DVIToSVGActions::preamble (const string &cmt) {
}


void DVIToSVGActions::postamble () {
}


/** This method is called when a "begin of page (bop)" command was found in the DVI file. 
 *  @param[in] c array with 10 components representing \count0 ... \count9. c[0] contains the
 *               current (printed) page number (may differ from page count) */
void DVIToSVGActions::beginPage (Int32 *c) {
	_svg.newPage(++_pageCount);
	_bbox = BoundingBox();  // clear bounding box
}


CharmapTranslator* DVIToSVGActions::getCharmapTranslator (const Font *font) const {
	if (font) {
		CharmapTranslatorMap::const_iterator it = _charmapTranslatorMap.find(font->uniqueFont());
		if (it != _charmapTranslatorMap.end())
			return it->second;
	}
	return 0;
}


/** This method is called when an "end of page (eop)" command was found in the DVI file. */
void DVIToSVGActions::endPage () {
	if (_pageMatrix)
		_svg.transformPage(*_pageMatrix);
	if (_bgcolor != Color::WHITE) {
		XMLElementNode *r = new XMLElementNode("rect");
		r->addAttribute("x", _bbox.minX());
		r->addAttribute("y", _bbox.minY());
		r->addAttribute("width", _bbox.width());
		r->addAttribute("height", _bbox.height());
		r->addAttribute("fill", _bgcolor.rgbString());
		_svg.prependToPage(r);
	}
}


void DVIToSVGActions::setBgColor (const Color &color) {
	_bgcolor = color;
}

