/*************************************************************************
** SVGTree.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <string>
#include "BoundingBox.h"
#include "DVIToSVG.h"
#include "Font.h"
#include "FontManager.h"
#include "SVGTree.h"
#include "XMLDocument.h"
#include "XMLDocTypeNode.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


// static class variables
bool SVGTree::CREATE_STYLE=true;
bool SVGTree::USE_FONTS=true;
bool SVGTree::CREATE_USE_ELEMENTS=false;


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


/** Sets the bounding box of the document. 
 *  @param[in] bbox bounding box in TeX point units */
void SVGTree::setBBox (const BoundingBox &bbox) {
	double bp = 72.0/72.27;  // pt -> bp
	ScalingMatrix scale(bp, bp);
	BoundingBox scaled_box = bbox;
	scaled_box.transform(scale);
	_root->addAttribute("width", XMLString(scaled_box.width())+"pt");
	_root->addAttribute("height", XMLString(scaled_box.height())+"pt");
	_root->addAttribute("viewBox", scaled_box.toSVGViewBox());
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
 *  @param[in] font font to be used */
void SVGTree::appendChar (int c, double x, double y, const Font &font) {
	XMLElementNode *node=_span;
	if (USE_FONTS) {
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
		node->append(XMLString(font.unicode(c), false));
	}
	else {
		if (_color.changed() || _matrix.changed()) {
			bool set_color = (_color.changed() && _color.get() != Color::BLACK);
			bool set_matrix = (_matrix.changed() && !_matrix.get().isIdentity());
			if (set_color || set_matrix) {
				_span = new XMLElementNode("g");
				if (_color.get() != Color::BLACK)
					_span->addAttribute("fill", _color.get().rgbString());
				if (!_matrix.get().isIdentity())
					_span->addAttribute("transform", _matrix.get().getSVG());
				_page->append(_span);
				node = _span;
				_color.changed(false);
				_matrix.changed(false);
			}
			else if (_color.get() == Color::BLACK && _matrix.get().isIdentity())
				node = _span = 0;
		}

		if (!node)
			node = _page;
		if (CREATE_USE_ELEMENTS) {
			ostringstream oss;
			oss << "#g" << FontManager::instance().fontID(_font) << '-' << c;
			XMLElementNode *use = new XMLElementNode("use");
			use->addAttribute("x", XMLString(x));
			use->addAttribute("y", XMLString(y));
			use->addAttribute("xlink:href", oss.str());
			node->append(use);
		}
		else {
			Glyph glyph;
			const PhysicalFont *font = dynamic_cast<const PhysicalFont*>(_font.get());
			if (font && font->getGlyph(c, glyph)) {
				double sx = font->scaledSize()/font->unitsPerEm();
				double sy = -sx;
				ostringstream oss;
				glyph.writeSVG(oss, sx, sy, x, y);
				XMLElementNode *glyph_node = new XMLElementNode("path");
				glyph_node->addAttribute("d", oss.str());
				node->append(glyph_node);
			}
		}
	}
}


/** Creates a new text element. This is a helper function used by appendChar().
 *  @param[in] x current x coordinate
 *  @param[in] y current y coordinate */
void SVGTree::newTextNode (double x, double y) {
	_text = new XMLElementNode("text");
	_span = 0; // no tspan in text element yet
	if (USE_FONTS) {
		const Font *font = _font.get();
		if (CREATE_STYLE || !font)
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


void SVGTree::transformPage (const Matrix *usermatrix) {
	double bp = 72.0/72.27;  // pt -> bp
	ScalingMatrix matrix(bp, bp);
	if (usermatrix)
		matrix.rmultiply(*usermatrix);
	if (!matrix.isIdentity())
		_page->addAttribute("transform", matrix.getSVG());
}


/** Creates an SVG element for a single glyph.
 *  @param[in] c character number
 *  @param[in] font font to extract the glyph from
 *  @return pointer to element node if glyph exists, 0 otherwise */
static XMLElementNode* createGlyphNode (int c, const PhysicalFont &font, GFGlyphTracer::Callback *cb) {
	Glyph glyph;
	if (!font.getGlyph(c, glyph, cb) || (!SVGTree::USE_FONTS && !SVGTree::CREATE_USE_ELEMENTS))
		return 0;

	double sx=1.0, sy=1.0;
	double upem = font.unitsPerEm();
	XMLElementNode *glyph_node=0;
	if (SVGTree::USE_FONTS) {
		glyph_node = new XMLElementNode("glyph");
		glyph_node->addAttribute("unicode", XMLString(font.unicode(c), false));
		glyph_node->addAttribute("horiz-adv-x", XMLString(font.hAdvance(c)));
		string name = font.glyphName(c);
		if (!name.empty())
			glyph_node->addAttribute("glyph-name", name);
	}
	else {
		ostringstream oss;
		oss << 'g' << FontManager::instance().fontID(&font) << '-' << c;
		glyph_node = new XMLElementNode("path");
		glyph_node->addAttribute("id", oss.str());
		sx = font.scaledSize()/upem;
		sy = -sx;
	}
	ostringstream oss;
	glyph.writeSVG(oss, sx, sy);
	glyph_node->addAttribute("d", oss.str());
	return glyph_node;
}


void SVGTree::appendFontStyles (const set<const Font*> &fonts) {
	if (CREATE_STYLE && USE_FONTS && !fonts.empty() && _defs) {
		XMLElementNode *styleNode = new XMLElementNode("style");
		styleNode->addAttribute("type", "text/css");
		_root->insertAfter(styleNode, _defs);
		typedef map<int, const Font*> SortMap;
		SortMap sortmap;
		FORALL(fonts, set<const Font*>::const_iterator, it)
			if (!dynamic_cast<const VirtualFont*>(*it))   // skip virtual fonts
				sortmap[FontManager::instance().fontID(*it)] = *it;
		ostringstream style;
		// add font style definitions in ascending order
		FORALL(sortmap, SortMap::const_iterator, it) {
			style << "text.f"     << it->first << ' '
				<< "{font-family:" << it->second->name()
				<< ";font-size:"   << it->second->scaledSize() << "}\n";
		}
		XMLCDataNode *cdata = new XMLCDataNode(style.str());
		styleNode->append(cdata);
	}
}


/** Appends glyph definitions of a given font to the defs section of the SVG tree.
 *  @param[in] font font to be appended
 *  @param[in] chars codes of the characters whose glyph outlines should be appended */
void SVGTree::append (const PhysicalFont &font, const set<int> &chars, GFGlyphTracer::Callback *cb) {
	if (chars.empty())
		return;

	if (USE_FONTS) {
		XMLElementNode *fontNode = new XMLElementNode("font");
		string fontname = font.name();
		fontNode->addAttribute("id", fontname);
		fontNode->addAttribute("horiz-adv-x", XMLString(font.hAdvance()));
		appendToDefs(fontNode);

		XMLElementNode *faceNode = new XMLElementNode("font-face");
		faceNode->addAttribute("font-family", fontname);
		faceNode->addAttribute("units-per-em", XMLString(font.unitsPerEm()));
		if (font.type() != PhysicalFont::MF) {
			faceNode->addAttribute("ascent", XMLString(font.ascent()));
			faceNode->addAttribute("descent", XMLString(font.descent()));
		}
		fontNode->append(faceNode);

		// add required missing-glyph element
		XMLElementNode *missing = new XMLElementNode("missing-glyph");
		missing->addAttribute("d", "");
		fontNode->append(missing);
		FORALL(chars, set<int>::const_iterator, i)
			fontNode->append(createGlyphNode(*i, font, cb));
	}
	else if (CREATE_USE_ELEMENTS && &font != font.uniqueFont()) {
		// If the same character is used in various sizes we don't want to embed the complete (lengthy) path
		// description multiple times because they would only differ by a scale factor. Thus it's better to
		// reference the already embedded path together with a transformation attribute and let the SVG renderer
		// scale the glyph properly. This is only necessary if we don't want to use font but path elements.
		FORALL(chars, set<int>::const_iterator, it) {
			ostringstream oss;
			XMLElementNode *use = new XMLElementNode("use");
			oss << 'g' << FontManager::instance().fontID(&font) << '-' << *it;
			use->addAttribute("id", oss.str());
			oss.str("");
			oss << "#g" << FontManager::instance().fontID(font.uniqueFont()) << '-' << *it;
			use->addAttribute("xlink:href", oss.str());
			double scale = font.scaledSize()/font.uniqueFont()->scaledSize();
			if (scale != 1.0) {
				oss.str("");
				oss << "scale(" << scale << ')';
				use->addAttribute("transform", oss.str());
			}
			appendToDefs(use);
		}
	}
	else {
		FORALL(chars, set<int>::const_iterator, i)
			appendToDefs(createGlyphNode(*i, font, cb));
	}
}

