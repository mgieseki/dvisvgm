/*************************************************************************
** GFGlyphTracer.cpp                                                    **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "GFGlyphTracer.h"
#include "Pair.h"

using namespace std;



GFGlyphTracer::GFGlyphTracer (istream &is, double upp) : GFTracer(is, upp)
{
	_glyph = new Glyph;
	_transfered = false;
}


GFGlyphTracer::~GFGlyphTracer () {
	if (!_transfered)
		delete _glyph;
}


void GFGlyphTracer::moveTo (double x, double y) {
	_glyph->moveto(x, y);
}


void GFGlyphTracer::lineTo (double x, double y) {
	_glyph->lineto(x, y);
}


void GFGlyphTracer::curveTo (double c1x, double c1y, double c2x, double c2y, double x, double y) {
	_glyph->cubicto(c1x, c1y, c2x, c2y, x, y);
}


void GFGlyphTracer::closePath () {
	_glyph->closepath();
}


void GFGlyphTracer::endChar (UInt32 c) {
	if (_transfered) {
		_glyph = new Glyph;
		_transfered = false;
	}
	else
		_glyph->newpath();
	GFTracer::endChar(c);
}


/** Returns a pointer to the current glyph object and transfers its ownership. That means
 *  the caller is responsible for deleting the object after its usage. GFGlyphTracer doesn't
 *  care for it any longer. To just retrieve the glyph without transferring ownership, use
 *  getGlyph(). */
Glyph* GFGlyphTracer::transferGlyph () {
	_transfered = true;
	return _glyph;
}

