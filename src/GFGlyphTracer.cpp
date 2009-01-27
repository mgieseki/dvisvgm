/***********************************************************************
** GFGlyphTracer.cpp                                                  **
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

#include "GFGlyphTracer.h"
#include "Pair.h"

using namespace std;


void GFGlyphTracer::moveTo (double x, double y) {
	LPair p(x, y);
	_glyph.addCommand(new GlyphMoveTo(p));
}


void GFGlyphTracer::lineTo (double x, double y) {
	LPair p(x, y);
	_glyph.addCommand(new GlyphLineTo(p));
}


void GFGlyphTracer::curveTo (double c1x, double c1y, double c2x, double c2y, double x, double y) {
	LPair p1(c1x, c1y);
	LPair p2(c2x, c2y);
	LPair p3(x, y);
	_glyph.addCommand(new GlyphCubicTo(p1, p2, p3));
}


void GFGlyphTracer::closePath () {
	_glyph.addCommand(new GlyphClosePath());
}


void GFGlyphTracer::endChar (UInt32 c) {
	_glyph.clear();
	GFTracer::endChar(c);
}
