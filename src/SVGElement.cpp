/*************************************************************************
** SVGElement.cpp                                                       **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2021 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "Color.hpp"
#include "Matrix.hpp"
#include "Opacity.hpp"
#include "SVGElement.hpp"
#include "XMLString.hpp"

using namespace std;


void SVGElement::setClipPathUrl (const string &url) {
	if (!url.empty())
		addAttribute("clip-path", "url(#"+url+")");
}


void SVGElement::setClipRule (FillRule rule) {
	if (rule != FR_NONZERO)
		addAttribute("clip-rule", "evenodd");
}


void SVGElement::setFillColor (Color color) {
	if (color != Color::BLACK)
		addAttribute("fill", color.svgColorString());
}


void SVGElement::setFillOpacity (const Opacity &opacity) {
	if (!opacity.isFillDefault()) {
		if (!opacity.fillalpha().isOpaque())
			addAttribute("fill-opacity", opacity.fillalpha().value());
		if (opacity.blendMode() != Opacity::BM_NORMAL)
			addAttribute("style", "mix-blend-mode:"+opacity.cssBlendMode());
	}
}


void SVGElement::setFillRule (FillRule rule) {
	if (rule != FR_NONZERO)
		addAttribute("fill-rule", "evenodd");
}


void SVGElement::setFillPatternUrl (const std::string &url) {
	if (!url.empty())
		addAttribute("fill", "url(#" + url + ")");
}


void SVGElement::setNoFillColor () {
	addAttribute("fill", "none");
}


void SVGElement::setStrokeColor (Color color) {
	addAttribute("stroke", color.svgColorString());
}


void SVGElement::setStrokeDash (const vector<double> &pattern, double offset) {
	if (!pattern.empty()) {
		string patternStr;
		for (double dashValue : pattern)
			patternStr += XMLString(dashValue)+",";
		patternStr.pop_back();
		addAttribute("stroke-dasharray", patternStr);
		if (offset != 0)
			addAttribute("stroke-dashoffset", XMLString(offset));
	}
}


void SVGElement::setStrokeLineCap (LineCap cap) {
	if (cap != LC_BUTT)
		addAttribute("stroke-linecap", cap == LC_ROUND ? "round" : "square");
}


void SVGElement::setStrokeLineJoin (LineJoin join) {
	if (join != LJ_MITER)
		addAttribute("stroke-linejoin", join == LJ_BEVEL ? "bevel" : "round");
}


void SVGElement::setStrokeMiterLimit (double limit) {
	if (limit != 4)
		addAttribute("stroke-miterlimit", XMLString(limit));
}


void SVGElement::setStrokeOpacity (const Opacity &opacity) {
	if (!opacity.isStrokeDefault()) {
		if (!opacity.strokealpha().isOpaque())
			addAttribute("stroke-opacity", opacity.strokealpha().value());
		if (opacity.blendMode() != Opacity::BM_NORMAL)
			addAttribute("style", "mix-blend-mode:"+opacity.cssBlendMode());
	}
}


void SVGElement::setStrokeWidth (double width) {
	if (width != 1)
		addAttribute("stroke-width", XMLString(width));
}


void SVGElement::setTransform (const Matrix &matrix) {
	if (!matrix.isIdentity())
		addAttribute("transform", matrix.toSVG());
}