/***********************************************************************
** GFTracer.cpp                                                       **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id$

#include <iostream>
#include <fstream>
#include <sstream>
#include "FontGlyph.h"
#include "GFTracer.h"

#ifdef __MSVC__
#include <potracelib.h>
#else
extern "C" {
#include <potracelib.h>
}
#endif

using namespace std;


GFTracer::GFTracer (istream &is, double upp) 
	: GFReader(is), unitsPerPoint(upp)
{
}


GFTracer::~GFTracer () {
}


void GFTracer::beginChar (UInt32 c) {
//	cerr << '[' << c;
}


void GFTracer::endChar (UInt32 c) {
	const Bitmap &bitmap = getBitmap();
	if (bitmap.isEmpty())
		return;
	// prepare potrace's bitmap structure
	potrace_bitmap_t pobitmap;
	pobitmap.w = bitmap.getWidth();
	pobitmap.h = bitmap.getHeight();
	pobitmap.dy = bitmap.copy(pobitmap.map);
	potrace_param_t *param = potrace_param_default();
	potrace_state_t *state = potrace_trace(param, &pobitmap);
	potrace_param_free(param);
	delete [] pobitmap.map;

	if (!state || state->status == POTRACE_STATUS_INCOMPLETE)
		cerr << "couln't completely trace character\n";
	else {
		glyph.clear();
		double hsf = unitsPerPoint/getHPixelsPerPoint();  // horizontal scale factor
		double vsf = unitsPerPoint/getVPixelsPerPoint();  // vertical scale factor
		for (potrace_path_t *path = state->plist; path; path = path->next) {
			potrace_dpoint_t &p = path->curve.c[path->curve.n-1][2]; // start/end point
			glyph.addCommand(new GlyphMoveTo(LPair(hsf*(p.x+bitmap.getXShift()), vsf*(p.y+bitmap.getYShift()))));
			for (int i=0; i < path->curve.n; i++) {
				if (path->curve.tag[i] == POTRACE_CURVETO) {
					LPair p1(hsf*(path->curve.c[i][0].x+bitmap.getXShift()), vsf*(path->curve.c[i][0].y+bitmap.getYShift()));
					LPair p2(hsf*(path->curve.c[i][1].x+bitmap.getXShift()), vsf*(path->curve.c[i][1].y+bitmap.getYShift()));
					LPair p3(hsf*(path->curve.c[i][2].x+bitmap.getXShift()), vsf*(path->curve.c[i][2].y+bitmap.getYShift()));
					glyph.addCommand(new GlyphCubicTo(p1, p2, p3));
				}
				else {
					LPair p2(hsf*(path->curve.c[i][1].x+bitmap.getXShift()), vsf*(path->curve.c[i][1].y+bitmap.getYShift()));
					LPair p3(hsf*(path->curve.c[i][2].x+bitmap.getXShift()), vsf*(path->curve.c[i][2].y+bitmap.getYShift()));
					glyph.addCommand(new GlyphLineTo(p2));
					glyph.addCommand(
							i == path->curve.n-1 
							? (GlyphCommand*)new GlyphClosePath() 
						   : (GlyphCommand*)new GlyphLineTo(p3));
				}
			}
		}
	}	
	potrace_state_free(state);	
}	

