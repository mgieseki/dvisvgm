/***********************************************************************
** SpecialEmHandler.cpp                                               **
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

#include "SpecialEmHandler.h"
#include "InputBuffer.h"

using namespace std;


SpecialEmHandler::SpecialEmHandler () : _linewidth(0.4) {
}


void SpecialEmHandler::reset () {
	_points.clear();
	_lines.clear();
}


static DPair unit_vector (char c, const DPair &v) {
	DPair uv;
	if (v.x() == 0 || c == 'v')
		uv.y(1);
	else if (v.y() == 0 || c == 'h')
		uv.x(1);
	else    // c == 'p'
		uv = v.ortho()/v.length();
	return uv;
}


static void create_line (const DPair &p1, const DPair &p2, char c1, char c2, double lw, SpecialActions *actions) {
	DPair v = p2-p1;
	DPair ov1 = unit_vector(c1, v)*(lw/2);
	DPair ov2 = unit_vector(c2, v)*(lw/2);

	DPair q11 = DPair(p1-ov1), q12 = DPair(p1+ov1);
	DPair q21 = DPair(p2-ov2), q22 = DPair(p2+ov2);
}


static double read_length (InputBuffer &in) {
	struct Unit {
		const char *name;
		double factor;
	} 
	units[] = {
		{"pt", 1}, 
		{"pc", 12}, 
		{"in", 72.27}, 
		{"bp", 72.27/72}, 
		{"cm", 72.27/2.54}, 
		{"mm", 72.27/25.4}, 
		{"dd", 1238.0/1157}, 
		{"cc", 1238.0/1157*12}, 
		{"sp", 1.0/65536},
		{0, 0}
	};
	double len = in.getDouble();
	in.skipSpace();
	for (Unit *p=units; p->name; p++)
		if (in.check(p->name)) {
			len *= p->factor;
			break;
		}
	return len;
}


void SpecialEmHandler::process (istream &is, SpecialActions *actions) {
	// em:moveto => move graphic cursor to dvi cursor
	// em:lineto => draw line from graphic cursor to dvi cursor, move graphic cursor to dvi cursor
	// em:linewidth <w> => set line width to <w>
	// em:point <n>[,<x>[,<y>]] => defines point <n> as (<x>,<y>); if <x> and/or <y> is missing, 
	//                             the corresponding dvi cursor coordinate is inserted
	//                             <x> and <y> are lengths 
	// em:line <n>[h|v|p], <m>[h|v|p] [,<w>] => draws line of width <w> from point #<n> to point #<m> 
	// 	point number suffixes: 
	// 	  h: cut line horizontally
	// 	  v: cut line vertically
	// 	  p: cut line orthogonally to line direction (default)
	//    if <w> is omitted, the global line width is used
	// 
	// supported length units: pt, pc, in, bp, cm, mm, dd, cc, sp 
	// default line width: 0.4pt
	
	if (!actions)
		return;

	StreamInputBuffer in(is, 128);
	string cmd = in.getWord();
	if (cmd == "moveto")
		_pos = DPair(actions->getX(), actions->getY());
	else if (cmd == "lineto") {
		DPair p(actions->getX(), actions->getY());
		create_line(_pos, p, 'p', 'p', _linewidth, actions);
		_pos = p;
	}
	else if (cmd == "linewidth")
		_linewidth = read_length(in);
	else if (cmd == "point") {
		DPair p(actions->getX(), actions->getY());
		int n = in.getInt();
		if (in.getPunct() == ',') {
			p.x(in.getDouble());
			if (in.getPunct() == ',')
				p.y(in.getDouble());
		}
		_points[n] = p;
	}
	else if (cmd == "line") {
		int n1 = in.getInt();
		int c1 = 'p';
		if (isalpha(in.peek())) 
			c1 = in.get();
		in.getPunct();
		int n2 = in.getInt();
		int c2 = 'p';
		if (isalpha(in.peek())) 
			c2 = in.get();
		double lw = _linewidth;
		if (in.getPunct() == ',')
			lw = in.getDouble();
		map<int,DPair>::iterator it1=_points.find(n1);
		map<int,DPair>::iterator it2=_points.find(n2);
		if (it1 != _points.end() && it2 != _points.end())
			create_line(it1->second, it2->second, c1, c2, lw, actions);
		else
			_lines.push_back(Line(n1, n2, c1, c2, lw));
	}
}
