/*************************************************************************
** DvisvgmSpecialHandler.cpp                                            **
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
#include "DvisvgmSpecialHandler.h"
#include "InputBuffer.h"
#include "InputReader.h"
#include "SpecialActions.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


/** Replaces constants of the form {?name} by their corresponding value.
 *  @param[in,out] str text to expand
 *  @param[in] actions interfcae to the world outside the special handler */
static void expand_constants (string &str, SpecialActions *actions) {
	struct Constant {
		const char *name;
		string val;
	}
	constants[] = {
		{"x", XMLString(actions->getX())},
		{"y", XMLString(actions->getY())},
		{"color", actions->getColor().rgbString()},
		{"nl", "\n"},
		{0, ""}
	};
	bool repl_bbox = true;
	while (repl_bbox) {
		size_t pos = str.find(string("{?bbox "));
		if (pos == string::npos)
			repl_bbox = false;
		else {
			size_t endpos = pos+7;
			while (endpos < str.length() && isalnum(str[endpos]))
				++endpos;
			if (str[endpos] == '}') {
				BoundingBox &box=actions->bbox(str.substr(pos+7, endpos-pos-7));
				str.replace(pos, endpos-pos+1, box.toSVGViewBox());
			}
			else
				repl_bbox = false;
		}
	}
	for (const Constant *p=constants; p->name; p++) {
		const string pattern = string("{?")+p->name+"}";
		size_t pos = str.find(pattern);
		while (pos != string::npos) {
			str.replace(pos, strlen(p->name)+3, p->val);
			pos = str.find(pattern, pos+p->val.length());  // look for further matches
		}
	}
}


/** Embeds the virtual rectangle (x, y ,w , h) into the current bounding box,
 *  where (x,y) is the lower left vertex composed of the current DVI position.
 *  @param[in] w width of the rectangle in TeX point units
 *  @param[in] h height of the rectangle in TeX point units
 *  @param[in] d depth of the rectangle in TeX point units */
static void update_bbox (double w, double h, double d, SpecialActions *actions) {
	double x = actions->getX();
	double y = actions->getY();
	actions->embed(BoundingBox(x, y, x+w, y-h));
	actions->embed(BoundingBox(x, y, x+w, y+d));
}


/** Inserts raw text into the SVG tree.
 *  @param[in,out] in the raw text is read from this input buffer
 *  @param[in] actions interfcae to the world outside the special handler
 *  @param[in] group true if the text should be wrapped by a group element */
static void raw (InputReader &in, SpecialActions *actions, bool group=false) {
	string str;
	while (!in.eof()) {
		int c = in.get();
		if (isprint(c))
			str += char(c);
	}
	expand_constants(str, actions);
	if (!str.empty()) {
		XMLNode *node = new XMLTextNode(str);
		if (group) {
			XMLElementNode *g = new XMLElementNode("g");
			g->addAttribute("x", actions->getX());
			g->addAttribute("y", actions->getY());
			if (actions->getColor() != Color::BLACK)
				g->addAttribute("fill", actions->getColor().rgbString());
			g->append(node);
			node = g;
		}
		actions->appendToPage(node);
	}
}


/** Evaluates the special dvisvgm:bbox.
 *  variant 1: dvisvgm:bbox [r[el]] <width> <height> [<depth>]
 *  variant 2: dvisvgm:bbox a[bs] <x1> <y1> <x2> <y2>
 *  variant 3: dvisvgm:bbox f[ix] <x1> <y1> <x2> <y2>
 *  variant 4: dvisvgm:bbox n[ew] <name> */
static void bbox (InputReader &in, SpecialActions *actions) {
	in.skipSpace();
	int c = in.peek();
	if (isalpha(c)) {
		while (!isspace(in.peek()))  // skip trailing characters
			in.get();
		if (c == 'n') {
			in.skipSpace();
			string name;
			while (isalnum(in.peek()))
				name += char(in.get());
			in.skipSpace();
			if (!name.empty() && in.eof())
				actions->bbox(name, true); // create new user box
		}
		else if (c == 'a' || c == 'f') {
			double p[4];
			for (int i=0; i < 4; i++)
				p[i] = in.getDouble();
			BoundingBox b(p[0], p[1], p[2], p[3]);
			if (c == 'a')
				actions->embed(b);
			else {
				actions->bbox() = b;
				actions->bbox().lock();
			}
		}
	}
	else
		c = 'r';   // no mode specifier => relative box parameters

	if (c == 'r') {
		double w = in.getDouble();
		double h = in.getDouble();
		double d = in.getDouble();
		update_bbox(w, h, d, actions);
	}
}


static void img (InputReader &in, SpecialActions *actions) {
	double w = in.getDouble();
	double h = in.getDouble();
	string f = in.getString();
	update_bbox(w, h, 0, actions);
	XMLElementNode *img = new XMLElementNode("image");
	img->addAttribute("x", actions->getX());
	img->addAttribute("y", actions->getY());
	img->addAttribute("width", w);
	img->addAttribute("height", h);
	img->addAttribute("xlink:href", f);
	if (actions && !actions->getMatrix().isIdentity())
		img->addAttribute("transform", actions->getMatrix().getSVG());
	actions->appendToPage(img);
}


/** Evaluates and executes a dvisvgm special statement.
 *  @param[in] prefix special prefix read by the SpecialManager
 *  @param[in] is the special statement is read from this stream
 *  @param[in,out] in the raw text is read from this input buffer */
bool DvisvgmSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	if (actions) {
		StreamInputBuffer ib(is);
		BufferInputReader in(ib);
		string cmd = in.getWord();
		if (cmd == "raw")               // raw <text>
			raw(in, actions);
		else if (cmd == "bbox")         // bbox [r] <width> <height> <depth> or bbox a <x1> <y1> <x2> <y2>
			bbox(in, actions);
		else if (cmd == "img") {        // img <width> <height> <file>
			img(in, actions);
		}
	}
	return true;
}


const char** DvisvgmSpecialHandler::prefixes () const {
	static const char *pfx[] = {"dvisvgm:", 0};
	return pfx;
}
