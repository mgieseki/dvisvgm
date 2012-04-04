/*************************************************************************
** DVIToSVGActions.cpp                                                  **
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

#include <cstring>
#include <ctime>
#include "BoundingBox.h"
#include "DVIToSVG.h"
#include "DVIToSVGActions.h"
#include "Font.h"
#include "FontManager.h"
#include "GlyphTracerMessages.h"
#include "SpecialManager.h"
#include "System.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


double DVIToSVGActions::PROGRESSBAR_DELAY=1000;  // initial delay in seconds (values >= 1000 disable the progressbar)
bool DVIToSVGActions::EXACT_BBOX = false;


DVIToSVGActions::DVIToSVGActions (DVIToSVG &dvisvg, SVGTree &svg)
	: _svg(svg), _dvisvg(dvisvg), _pageMatrix(0), _bgcolor(Color::WHITE), _boxes(0)
{
	_currentFontNum = -1;
	_pageCount = 0;
}


DVIToSVGActions::~DVIToSVGActions () {
	delete _pageMatrix;
	delete _boxes;
}


void DVIToSVGActions::reset() {
	_usedChars.clear();
	_usedFonts.clear();
	_bbox = BoundingBox();
	_currentFontNum = -1;
	_bgcolor = Color::WHITE;
}


void DVIToSVGActions::setPageMatrix (const Matrix &matrix) {
	delete _pageMatrix;
	_pageMatrix = new Matrix(matrix);
}


void DVIToSVGActions::moveToX (double x) {
	_dvisvg.specialManager().notifyPositionChange(getX(), getY());
	_svg.setX(getX());        // get current horizontal DVI position
}


void DVIToSVGActions::moveToY (double y) {
	_dvisvg.specialManager().notifyPositionChange(getX(), getY());
	_svg.setY(y);
}


/** This method is called when a "set char" command was found in the DVI file.
 *  It draws a character of the current font.
 *  @param[in] x horizontal position of left bounding box edge
 *  @param[in] y vertical position of the character's baseline
 *  @param[in] c character code relative to the current font
 *  @param[in] font font to be used */
void DVIToSVGActions::setChar (double x, double y, unsigned c, const Font *font) {
	// If we use SVG fonts there is no need to record all font name/char/size combinations
	// because the SVG font mechanism handles this automatically. It's sufficient to
	// record font names and chars. The various font sizes can be ignored here.
	// For a given font object, Font::uniqueFont() returns the same unique font object for
	// all fonts with the same name.
	_usedChars[SVGTree::USE_FONTS ? font->uniqueFont() : font].insert(c);

	// However, we record all required fonts
	_usedFonts.insert(font);

	_svg.appendChar(c, x, y, *font);
	// update bounding box
	double wl=0, wr=0, h=0, d=0; // left/right width, height, and depth of character c
	static string fontname;
	GlyphTracerMessages callback(fontname != font->name(), false);
	fontname = font->name();
	BoundingBox charbox;
	const PhysicalFont *ph_font = dynamic_cast<const PhysicalFont*>(font);
	if (EXACT_BBOX && ph_font && ph_font->getGlyphBox(c, charbox, &callback)) {
		if ((wl = charbox.minX()) > 0) wl=0;
		if ((wr = charbox.maxX()) < 0) wr=0;
		if ((h = charbox.maxY()) < 0) h=0;
		if ((d = -charbox.minY()) < 0) d=0;
	}
	else {
		double s = font->scaleFactor();
		wr = s*(font->charWidth(c) + font->italicCorr(c));
		h  = s*font->charHeight(c);
		d  = s*font->charDepth(c);
	}

	BoundingBox bbox(x+wl, y-h, x+wr, y+d);
/*	XMLElementNode *rect = new XMLElementNode("rect");
	rect->addAttribute("x", x+wl);
	rect->addAttribute("y", y-h);
	rect->addAttribute("width", (-wl+wr));
	rect->addAttribute("height", (h+d));
	rect->addAttribute("fill", "none");
	rect->addAttribute("stroke", "red");
	rect->addAttribute("stroke-width", "0.5");
	_svg.appendToPage(rect);*/
	if (!getMatrix().isIdentity())
		bbox.transform(getMatrix());
	embed(bbox);
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
	BoundingBox bb(x, y-height, x+width, y);
	if (!getMatrix().isIdentity())
		bb.transform(getMatrix());
	embed(bb);
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
		Message::estream(true) << "error in special '" << s << "': " << e.what() << '\n';
	}
}


void DVIToSVGActions::beginSpecial (const char *prefix) {
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
void DVIToSVGActions::beginPage (unsigned n, Int32 *c) {
	_svg.newPage(++_pageCount);
	_bbox = BoundingBox();  // clear bounding box
	if (_boxes)
		_boxes->clear();
}


/** This method is called when an "end of page (eop)" command was found in the DVI file. */
void DVIToSVGActions::endPage () {
	_svg.transformPage(_pageMatrix);
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


void DVIToSVGActions::embed(const BoundingBox& bbox) {
	_bbox.embed(bbox);
	if (_boxes) {
		FORALL(*_boxes, BoxMap::iterator, it)
			it->second.embed(bbox);
	}
}


void DVIToSVGActions::embed(const DPair& p, double r) {
	if (r == 0)
		_bbox.embed(p);
	else
		_bbox.embed(p, r);
	if (_boxes)
		FORALL(*_boxes, BoxMap::iterator, it)
			it->second.embed(p, r);
}


BoundingBox& DVIToSVGActions::bbox(const string& name, bool reset) {
	if (!_boxes)
		_boxes = new BoxMap;
	BoundingBox &box = (*_boxes)[name];
	if (reset)
		box = BoundingBox();
	return box;
}


/** This method is called by subprocesses like the PS handler when
 *  a computation step has finished. Since the total number of steps
 *  can't be determined in advance, we don't show a percent value but
 *  a rotating dash. */
void DVIToSVGActions::progress (const char *id) {
	if (PROGRESSBAR_DELAY < 1000) {
		static double time=0;
		// slow down updating of the progress indicator to prevent flickering
		if (System::time() - time > 0.1) {
			progress(0, 0, id);
			time = System::time();
		}
	}
}


/** Returns the number of digits of a given integer. */
static int digits (int n) {
	if (n == 0)
		return 1;
	if (n > 0)
		return int(log10(double(n))+1);
	return int(log10(double(-n))+2);
}


/** Draws a simple progress indicator.
 *  @param[in] current current iteration step (of 'total' steps)
 *  @param[in] total total number of iteration steps
 *  @param[in] id ID of the subprocess providing the information */
void DVIToSVGActions::progress (size_t current, size_t total, const char *id) {
	static double time=0;
	static bool draw=false; // show progress indicator?
	static int step = -1;   // >=0: rotating dash
	static size_t prev_current=0, prev_total=1;
	static const char *prev_id=0;
	const char *tips = "-\\|/";
	if (current == 0 && total > 0) {
		time = System::time();
		draw = false;
		Message::mstream() << '\n';
	}
	// don't show the progress indicator before the given time has elapsed
	if (!draw && System::time()-time > PROGRESSBAR_DELAY)
		draw = true;
	if (draw && (System::time() - time > 0.1 || (total > 0 && current == total) || prev_id != id)) {
		if (total == 0) {
			current = prev_current;
			total = prev_total;
			step = (step+1) % 4;
		}
		else {
			prev_current = current;
			prev_total = total;
			step = -1;
		}
		// adapt length of progress indicator to terminal width
		int cols = Terminal::columns();
		int width = (cols == 0 || cols > 60) ? 50 : 49-60+cols;
		double factor = double(current)/double(total);
		int length = int(width*factor);
		Message::mstream(false, Message::MC_PROGRESS)
			<< '[' << string(length, '=')
			<< (factor < 1.0 ? (step < 0 ? ' ' : tips[step]) : '=')
			<< string(width-length, ' ')
			<< "] " << string(3-digits(int(100*factor)), ' ') << int(100*factor)
			<< "%\r";
		// overprint indicator when finished
		if (factor == 1.0)
			Message::estream().clearline();
		time = System::time();
		prev_id = id;
	}
}
