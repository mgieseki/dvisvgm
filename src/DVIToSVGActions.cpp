/***********************************************************************
** DVIToSVGActions.cpp                                                **
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

#include "BoundingBox.h"
#include "CharmapTranslator.h"
#include "DVIReader.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


DVIToSVGActions::DVIToSVGActions (const DVIReader &reader, XMLElementNode *svgelem) 
	: dviReader(reader),
     xmoved(false), ymoved(false), processSpecials(false), pageCount(0), 
	  currentFont(-1),
	  svgElement(svgelem), pageElement(0), styleElement(0), charElement(0),
	  fileFinder(0), transMatrix(0)
{
}


DVIToSVGActions::~DVIToSVGActions () {
	delete transMatrix;
	FORALL (charmapTranslatorMap, CharmapTranslatorMap::iterator, i)
		delete i->second;
}


void DVIToSVGActions::setTransformation (const TransformationMatrix &matrix) {
	transMatrix = new TransformationMatrix(matrix);
}

/** This method is called when a "set char" command was found in the DVI file. 
 *  It draws a character of the current font.
 *  @param x horizontal position of left bounding box edge 
 *  @param y vertical position of the character's baseline 
 *  @param c character code relative to the current font */
void DVIToSVGActions::setChar (double x, double y, unsigned c, const Font *font) {
	if (dynamic_cast<const VirtualFont*>(font))
		return;
	x *= BP;
	y *= BP;
	string fontname = font->name();
	const CharmapTranslator *cmt = charmapTranslatorMap[fontname];
	usedCharsMap[fontname].insert(c);
	XMLTextNode *textNode = new XMLTextNode(XMLString(cmt->unicode(c), false));	

	// create a new tspan element with positioning information
	// if "cursor" was moved
	if (xmoved || ymoved) {
		charElement = new XMLElementNode("tspan");
		if (xmoved)
			charElement->addAttribute("x", XMLString(x));
		if (ymoved)
			charElement->addAttribute("y", XMLString(y));
		charElement->append(textNode);
		styleElement->append(charElement);
		xmoved = ymoved = false;
	}
	else if (charElement) // no explicit cursor movement => append text to existing node
		charElement->append(textNode);
	else                  // no tspan node and no cursor movement
		styleElement->append(textNode);
}


/** This method is called when a "set rule" or "put rule" command was found in the 
 *  DVI file. It draws a solid unrotated rectangle.
 *  @param x horizontal position of left edge 
 *  @param y vertical position of bottom(!) edge 
 *  @param height length of the vertical edges 
 *  @param width length of the horizontal edges */
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


void DVIToSVGActions::defineFont (int num, const string &path, const string &name, double ds, double sc) {
	if (charmapTranslatorMap.find(name) == charmapTranslatorMap.end()) 
		charmapTranslatorMap[name] = new CharmapTranslator(name.c_str());
}


/** This method is called when a "set font" command was found in the DVI file. The
 *  font must be previously defined.
 *  @param num unique number of the font in the DVI file */
void DVIToSVGActions::setFont (int num) {
	if (num != currentFont) {
		styleElement = new XMLElementNode("text");
		styleElement->addAttribute("class", string("f") + XMLString(num));
		styleElement->addAttribute("x", XMLString(dviReader.getXPos()*BP));
		styleElement->addAttribute("y", XMLString(dviReader.getYPos()*BP));
		pageElement->append(styleElement);
		charElement = 0;  // force creating a new charElement when adding next char
		xmoved = ymoved = false;
		currentFont = num;
	}
}


/** This method is called when a "special" command was found in the DVI file. */
void DVIToSVGActions::special (const string &s) {
	if (!processSpecials) 
		return;
	Message::wstream(true) << "\\special ignored: " << s << endl;
}


/** This method is called when the DVI preamble was read 
 *  @param cmt preamble comment text. */
void DVIToSVGActions::preamble (const string &cmt) {
}


void DVIToSVGActions::postamble () {
}


/** This method is called when a "begin of page (bop)" command was found in the DVI file. 
 *  @param c array with 10 components representing \count0 ... \count9. c[0] contains the
 *           current (printed) page number (may differ from page count) */
void DVIToSVGActions::beginPage (Int32 *c) {
	pageCount++;
	pageElement = new XMLElementNode("g");
	pageElement->addAttribute("id", string("page")+XMLString(int(pageCount)));
	if (transMatrix)
		pageElement->addAttribute("transform", transMatrix->getSVG());
	svgElement->append(pageElement);
	ostringstream oss;
	Message::mstream() << '[' << c[0];
	xmoved = ymoved = false;
}


CharmapTranslator* DVIToSVGActions::getCharmapTranslator (string fontname) const {
	CharmapTranslatorMap::const_iterator it = charmapTranslatorMap.find(fontname);
	if (it != charmapTranslatorMap.end())
		return it->second;
	return 0;
}

/** This method is called when an "end of page (eop)" command was found in the DVI file. */
void DVIToSVGActions::endPage () {
	Message::mstream() << ']';
}

