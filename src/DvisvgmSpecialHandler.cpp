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
#include "SpecialActions.h"
#include "XMLNode.h"
#include "XMLString.h"
#include "debug.h"

using namespace std;


static void replace_variables (string &str, SpecialActions *actions) {
	struct Variables {
		const char *var;
		XMLString val;
	} 
	variables[] = {
		{"x", XMLString(actions->getX())},
		{"y", XMLString(actions->getY())},
		{"color", actions->getColor().rgbString()},
		{0, ""}
	};
	for (Variables *p=variables; p->var; p++) {
		const string search = string("{?")+p->var+"}";
		size_t pos = str.find(search);
		while (pos != string::npos) {
			str.replace(pos, strlen(p->var)+3, p->val);
			pos = str.find(search, pos+p->val.length());
		}
	}
}



static void raw (StreamInputBuffer &in, SpecialActions *actions, bool group) {
	string str;
	while (!in.eof()) {
		char c = in.get();
		if (isprint(c))
			str += c;
	}
	replace_variables(str, actions);
	if (!str.empty()) {
		XMLNode *node = new XMLTextNode(str);
		if (group) {
			XMLElementNode *g = new XMLElementNode("g");
			g->addAttribute("x", actions->getX());
			g->addAttribute("y", actions->getY());
			if (actions->getColor() != 0)
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



bool DvisvgmSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	if (actions) {
		StreamInputBuffer in(is);
		string cmd = in.getWord();
		if (cmd == "raw")               // raw <text>
			raw(in, actions, false);
		else if (cmd == "bbox") {       // bbox <width> <height>
			double w = in.getDouble();
			double h = in.getDouble();
			update_bbox(w, h, actions);
		}
		else if (cmd == "img") {
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


