/***********************************************************************
** DvisvgmSpecialHandler.cpp                                          **
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
		XMLString  val;
	} 
	constants[] = {
		{"x", XMLString(actions->getX())},
		{"y", XMLString(actions->getY())},
		{"color", actions->getColor().rgbString()},
		{0, ""}
	};
	for (const Constant *p=constants; p->name; p++) {
		const string pattern = string("{?")+p->name+"}";
		size_t pos = str.find(pattern);
		while (pos != string::npos) {
			str.replace(pos, strlen(p->name)+3, p->val);
			pos = str.find(pattern, pos+p->val.length());  // look for further matches
		}
	}
}


/** Inserts raw text into the SVG tree. 
 *  @param[in,out] in the raw text is read from this input buffer
 *  @param[in] actions interfcae to the world outside the special handler 
 *  @param[in] group true if the text should be wrapped by a group element */
static void raw (InputReader &in, SpecialActions *actions, bool group=false) {
	string str;
	while (!in.eof()) {
		char c = in.get();
		if (isprint(c))
			str += c;
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


static void update_bbox (double w, double h, SpecialActions *actions) {
	double x = actions->getX();
	double y = actions->getY();
	actions->bbox().embed(BoundingBox(x, y, x+w, y+h));
}


/** Evaluates and executes a dvisvgm special statement.
 *  @param[in] prefix special prefix read by the SpecialManager
 *  @param[in] is the special statement is read from this stream 
 *  @param[in,out] in the raw text is read from this input buffer */
bool DvisvgmSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	SHOW(prefix);
	if (actions) {
		StreamInputBuffer ib(is);
		BufferInputReader in(ib);
		string cmd = in.getWord();
		if (cmd == "raw")               // raw <text>
			raw(in, actions);
		else if (cmd == "bbox") {       // bbox <width> <height>
			double w = in.getDouble();
			double h = in.getDouble();
			update_bbox(w, h, actions);
		}
		else if (cmd == "img") {        // img <width> <height> <file>
			double w = in.getDouble();
			double h = in.getDouble();
			string f = in.getString();
			update_bbox(w, h, actions);
			XMLElementNode *img = new XMLElementNode("image");
			img->addAttribute("x", actions->getX());
			img->addAttribute("y", actions->getY());
			img->addAttribute("width", w);
			img->addAttribute("height", h);
			img->addAttribute("xlink:href", f);
			actions->appendToPage(img);
		}
	}
	return true;
}


int DvisvgmSpecialHandler::prefixes (const char **p[]) const {
	static const char *pfx[] = {"dvisvgm:"};
	*p = pfx;
	return 1;
}
