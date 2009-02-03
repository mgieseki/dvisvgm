/***********************************************************************
** DVIToSVGActions.cpp                                                **
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

#include "BoundingBox.h"
#include "CharmapTranslator.h"
#include "DVIReader.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "SpecialManager.h"
#include "ColorSpecialHandler.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


DVIToSVGActions::DVIToSVGActions (const DVIReader &reader, XMLElementNode *svgelem) 
	: _dviReader(reader), _specialManager(0),
	  svgElement(svgelem), pageElement(0), styleElement(0), charElement(0),
	  _transMatrix(0)
{
	_xmoved = _ymoved = false;
	_currentFont = -1;
	_pageCount = 0;
}


DVIToSVGActions::~DVIToSVGActions () {
	delete _transMatrix;
	delete _specialManager;
	FORALL (_charmapTranslatorMap, CharmapTranslatorMap::iterator, i)
		delete i->second;
}


void DVIToSVGActions::setProcessSpecials (bool enable) {
	if (!enable) {
		delete _specialManager;
		_specialManager = 0;
	}
	else if (!_specialManager) {
		_specialManager = new SpecialManager;
		_specialManager->registerHandler(new ColorSpecialHandler);
	}
}


void DVIToSVGActions::setTransformation (const TransformationMatrix &matrix) {
	_transMatrix = new TransformationMatrix(matrix);
}


/** This method is called when a "set char" command was found in the DVI file. 
 *  It draws a character of the current font.
 *  @param[in] x horizontal position of left bounding box edge 
 *  @param[in] y vertical position of the character's baseline 
 *  @param[in] c character code relative to the current font 
 *  @param[in] font font to be used */
void DVIToSVGActions::setChar (double x, double y, unsigned c, const Font *font) {
	x *= BP;
	y *= BP;
	font = font->uniqueFont();
	const CharmapTranslator *cmt = _charmapTranslatorMap[font];
	_usedCharsMap[font].insert(c);
	XMLTextNode *textNode = new XMLTextNode(XMLString(cmt->unicode(c), false));	

	// create a new tspan element with positioning information
	// if "cursor" was moved
	if (_xmoved || _ymoved) {
		charElement = new XMLElementNode("tspan");
		if (_xmoved)
			charElement->addAttribute("x", XMLString(x));
		if (_ymoved)
			charElement->addAttribute("y", XMLString(y));
		charElement->append(textNode);
		styleElement->append(charElement);
		_xmoved = _ymoved = false;
	}
	else if (charElement) // no explicit cursor movement => append text to existing node
		charElement->append(textNode);
	else                  // no tspan node and no cursor movement
		styleElement->append(textNode);
}


/** This method is called when a "set rule" or "put rule" command was found in the 
 *  DVI file. It draws a solid unrotated rectangle.
 *  @param[in] x horizontal position of left edge 
 *  @param[in] y vertical position of bottom(!) edge 
 *  @param[in] height length of the vertical edges 
 *  @param[in] width length of the horizontal edges */
void DVIToSVGActions::setRule (double x, double y, double height, double width) {
	x *= BP;
	y *= BP;
	height *= BP;
	width  *= BP;
	// (x,y) is the lower left corner of the rectangle
	XMLElementNode *rect = new XMLElementNode("rect");
	rect->addAttribute("x", x);
	rect->addAttribute("y", y-height);
	rect->addAttribute("height", height);
	rect->addAttribute("width", width);
	pageElement->append(rect);
}


void DVIToSVGActions::defineFont (int num, const Font *font) {
	font = font->uniqueFont();
	if (_charmapTranslatorMap.find(font) == _charmapTranslatorMap.end()) {
		_charmapTranslatorMap[font] = new CharmapTranslator(font);
	}
}


/** This method is called when a "set font" command was found in the DVI file. The
 *  font must be previously defined.
 *  @param[in] num unique number of the font in the DVI file 
 *  @param[in] font pointer to the font object */
void DVIToSVGActions::setFont (int num, const Font *font) {
	if (num != _currentFont) {
		styleElement = new XMLElementNode("text");
		styleElement->addAttribute("class", string("f") + XMLString(num));
		styleElement->addAttribute("x", XMLString(_dviReader.getXPos()*BP));
		styleElement->addAttribute("y", XMLString(_dviReader.getYPos()*BP));
		pageElement->append(styleElement);
		charElement = 0;  // force creating a new charElement when adding next char
		_xmoved = _ymoved = false;
		_currentFont = num;
	}
}


/** This method is called when a "special" command was found in the DVI file. 
 *  @param[in] s the special expression */
void DVIToSVGActions::special (const string &s) {
	if (_specialManager) {
		try {
			_specialManager->process(s);
			// @@ output message in case of unsupported specials?
		}
		catch (const SpecialException &e) {
			Message::estream(true) << "error in special '" << s << "': " << e.getMessage() << endl;
		}
	}
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
	_pageCount++;
	pageElement = new XMLElementNode("g");
	pageElement->addAttribute("id", string("page")+XMLString(int(_pageCount)));
	if (_transMatrix)
		pageElement->addAttribute("transform", _transMatrix->getSVG());
	svgElement->append(pageElement);
	ostringstream oss;
	Message::mstream() << '[' << c[0];
	_xmoved = _ymoved = false;
}


CharmapTranslator* DVIToSVGActions::getCharmapTranslator (const Font *font) const {
	CharmapTranslatorMap::const_iterator it = _charmapTranslatorMap.find(font);
	if (it != _charmapTranslatorMap.end())
		return it->second;
	return 0;
}


/** This method is called when an "end of page (eop)" command was found in the DVI file. */
void DVIToSVGActions::endPage () {
	Message::mstream() << ']';
}

