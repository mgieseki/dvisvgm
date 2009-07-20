/***********************************************************************
** SVGTree.cpp                                                        **
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
#include "DVIToSVG.h"
#include "Font.h"
#include "FontManager.h"
#include "SVGTree.h"
#include "XMLDocument.h"
#include "XMLDocTypeNode.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


SVGTree::SVGTree () : _font(0), _color(Color::BLACK), _matrix(1) {
	_xchanged = _ychanged = false;
	_fontnum = 0;
	reset();
}


/** Clears the SVG tree and initializes the root element. */
void SVGTree::reset () {
	_doc.clear();
	_root = new XMLElementNode("svg");
	_root->addAttribute("version", "1.1");
	_root->addAttribute("xmlns", "http://www.w3.org/2000/svg");
	_root->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
	_doc.setRootNode(_root);
	_doc.append(new XMLDocTypeNode("svg", "PUBLIC", 
		"\"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\""));
	_page = _text = _span = _defs = 0;
}


/** Sets the bounding box of the document. */
void SVGTree::setBBox (const BoundingBox &bbox) {
	_root->addAttribute("width", XMLString(bbox.width())); 		
	_root->addAttribute("height", XMLString(bbox.height()));
	_root->addAttribute("viewBox", bbox.toSVGViewBox());
}


/** Starts a new page. 
 *  @param[in] pageno number of new page */
void SVGTree::newPage (int pageno) {
	_page = new XMLElementNode("g");
	if (pageno >= 0)
		_page->addAttribute("id", string("page")+XMLString(pageno));
	_root->append(_page);
	_text = _span = 0;
}


void SVGTree::appendToDefs (XMLNode *node) {
	if (!_defs) {
		_defs = new XMLElementNode("defs");
		_root->prepend(_defs);
	}
	_defs->append(node);
}

/** Appends a single charater to the current text node. If necessary, and depending on output mode 
 *  and further output states, new XML elements (text, tspan, g, ...) are created.
 *  @param[in] c character to be added
 *  @param[in] x x coordinate
 *  @param[in] y y coordinate
 *  @param[in] fontManager the FontManager provides information about the fonts used in the document
 *  @param[in] cmt the CharmapTranslator remaps the character codes */
void SVGTree::appendChar (int c, double x, double y, const FontManager &fontManager, const CharmapTranslator &cmt) {
	XMLElementNode *node=_span;
	if (DVIToSVG::USE_FONTS) {
		// changes of fonts and transformations require a new text element
		if (!_text || _font.changed() || _matrix.changed()) {
			newTextNode(x, y);
			node = _text;	
			_color.changed(true);
		}	
		if (_xchanged || _ychanged || (_color.changed() && _color.get() != Color::BLACK)) {
			// if drawing position was explicitly changed, create a new tspan element
			_span = new XMLElementNode("tspan");
			if (_xchanged) {
				_span->addAttribute("x", x);
				_xchanged = false;
			}
			if (_ychanged) {
				_span->addAttribute("y", y);
				_ychanged = false;
			}
			if (_color.get() != Color::BLACK) {
					_span->addAttribute("fill", _color.get().rgbString());
				_color.changed(false);
			}
			_text->append(_span);
			node = _span;
		}
		if (!node) {
			if (!_text)
				newTextNode(x, y);
			node = _text;
		}
		node->append(XMLString(cmt.unicode(c), false));
	}
	else {
		if (_color.changed() || _matrix.changed()) {
			bool set_color = (_color.changed() && _color.get() != Color::BLACK);
			bool set_matrix = (_matrix.changed() && !_matrix.get().isIdentity());
			if (set_color || set_matrix) {
				_span = new XMLElementNode("g");
				if (set_color)
					_span->addAttribute("fill", _color.get().rgbString());
				if (set_matrix)
					_span->addAttribute("transform", _matrix.get().getSVG());
				_page->append(_span);
				node = _span;
			}
			else
				node = _span = 0;
		}
			
		if (!node)
			node = _page;
		ostringstream oss;
		oss << "#g" << fontManager.fontID(_font) << c;
		XMLElementNode *use = new XMLElementNode("use");
		use->addAttribute("x", XMLString(x));
		use->addAttribute("y", XMLString(y));
		use->addAttribute("xlink:href", oss.str());
		node->append(use);
	}
}


/** Creates a new text element. This is a helper function used by appendChar(). 
 *  @param[in] x current x coordinate 
 *  @param[in] y current y coordinate */
void SVGTree::newTextNode (double x, double y) {	
	_text = new XMLElementNode("text");
	_span = 0; // no tspan in text element yet
	if (DVIToSVG::USE_FONTS) {
		const Font *font = _font.get();
		if (DVIToSVG::CREATE_STYLE || !font)
			_text->addAttribute("class", string("f")+XMLString(_fontnum));
		else {
			_text->addAttribute("font-family", font->name());
			_text->addAttribute("font-size", font->scaledSize());
		}
	}
	_text->addAttribute("x", x);
	_text->addAttribute("y", y);
	if (!_matrix.get().isIdentity())
		_text->addAttribute("transform", _matrix.get().getSVG());
	_page->append(_text);
	_font.changed(false);
	_matrix.changed(false);
	_xchanged = false;
	_ychanged = false;
}


void SVGTree::setFont (int num, const Font *font) {
	_font.set(font);
	_fontnum = num;
}


void SVGTree::transformPage (const Matrix &m) {
	_page->addAttribute("transform", m.getSVG());
}
