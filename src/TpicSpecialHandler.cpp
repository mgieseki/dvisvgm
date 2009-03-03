/***********************************************************************
** TpicSpecialHandler.cpp                                             **
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
#include <sstream>
#include "Color.h"
#include "InputBuffer.h"
#include "SpecialActions.h"
#include "TpicSpecialHandler.h"
#include "XMLNode.h"
#include "XMLString.h"
#include "types.h"

using namespace std;

#define cmd_id(c1,c2) ((c1 << 8) | c2)



TpicSpecialHandler::TpicSpecialHandler () {
	reset();
}


void TpicSpecialHandler::endPage () {
	reset();
}


void TpicSpecialHandler::reset () {
	_points.clear();
	_penwidth = 1.0;
	_fill = -1.0; // no fill
}


/** Creates SVG elements that draw lines through the recorded points. 
 *  @param[in] fill true if enclosed area should be filled 
 *  @param[in] ddist dash/dot distance of line in TeX point units
 *                   (0:solid line, >0:dashed line, <0:dotted line) */
void TpicSpecialHandler::drawLines (bool fill, double ddist, SpecialActions *actions) {
	if (actions) {
		XMLElementNode *elem=0;
		if (_points.size() == 1) {
			const DPair &p = _points.back();
			elem = new XMLElementNode("circle");
			elem->addAttribute("cx", p.x()+actions->getX());
			elem->addAttribute("cy", p.y()+actions->getY());
			elem->addAttribute("r", _penwidth/2.0);
			actions->bbox().embed(p, _penwidth/2.0);
		}
		else {
			if (_points.size() == 2 || (!fill && _points.front() != _points.back())) {
				elem = new XMLElementNode("polyline");
				elem->addAttribute("fill", "none");
			}
			else {
				if (_points.front() == _points.back())
					_points.pop_back();
				if (_fill < 0)
					_fill = 1;
				Color color = actions->getColor();
				color *= _fill;
				elem = new XMLElementNode("polygon");
				elem->addAttribute("fill", fill ? color.rgbString() : "none");
			}
			ostringstream oss;
			FORALL(_points, list<DPair>::iterator, it) {
				if (it != _points.begin())
					oss << ' ';
				double x = it->x()+actions->getX();
			  	double y = it->y()+actions->getY();
				oss << x << ',' << y;
				actions->bbox().embed(x, y);
			}
			elem->addAttribute("points", oss.str());
			elem->addAttribute("stroke", "black");
			elem->addAttribute("stroke-width", XMLString(_penwidth));
		}
		if (ddist > 0)
			elem->addAttribute("stroke-dasharray", XMLString(ddist));
		else if (ddist < 0) {
			ostringstream oss;
			oss << _penwidth << ' ' << -ddist;
			elem->addAttribute("stroke-dasharray", oss.str());
		}
		actions->appendToPage(elem);
	}
	reset();	
}


bool TpicSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	if (!prefix || strlen(prefix) != 2)
		return false;

	const double PT=0.07227; // factor for milli-inch to TeX points
	StreamInputBuffer in(is);
	switch (cmd_id(prefix[0], prefix[1])) {
		case cmd_id('p','n'): // set pen width in milli-inches
			_penwidth = in.getDouble()*PT;
			break;
		case cmd_id('b','k'): // set fill color to black
			_fill = 0;
			break;
		case cmd_id('w','h'): // set fill color to white
			_fill = 1;
			break;
		case cmd_id('s','h'): // set fill color to given gray level
			in.skipSpace();
			_fill = in.eof() ? 0.5 : max(0.0, min(1.0, in.getDouble()));
			break;
		case cmd_id('t','x'): // set fill pattern
			break;
		case cmd_id('p','a'): { // add point to path
			double x=in.getDouble()*PT;
			double y=in.getDouble()*PT;
			_points.push_back(DPair(x,y));
			break;
		}
		case cmd_id('f','p'): // draw solid lines through recorded points; close and fill path if fill color was defined
			drawLines(_fill >= 0, 0, actions);
			break;
		case cmd_id('i','p'): // as above but always fill polygon
			drawLines(true, 0, actions);
			break;
		case cmd_id('d','a'): // as fp but draw dashed lines
			drawLines(_fill >= 0, in.getDouble()*72.27, actions);
			break;
		case cmd_id('d','t'): // as fp but draw dotted lines
			drawLines(_fill >= 0, -in.getDouble()*72.27, actions);
			break;
		case cmd_id('s','p'): // draw quadratic splines through recorded points
			break;
		case cmd_id('a','r'): // draw elliptical arc
			break;
		case cmd_id('i','a'): // fill elliptical arc
			break;
		default:
			return false;
	}
	return true;
}


