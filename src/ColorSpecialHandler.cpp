/***********************************************************************
** ColorSpecialHandler.cpp                                            **
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

#include <algorithm>
#include <vector>
#include "ColorSpecialHandler.h"

using namespace std;


/** Approximates a CMYK color by an RGV triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] rgb  RGB approximation */
static void cmyk_to_rgb (const vector<double> &cmyk, vector<double> &rgb) {
	const double factor = (cmyk[3] > 0) ? -1 : 1;
	for (int i=0; i < 3; i++) {
		rgb[i] = 1.0-cmyk[i]*factor;
		if (rgb[i] < 0)
			rgb[i] = 0;
		else if (rgb[i] > 1)
			rgb[i] = 1;
	}
}


static void gray_to_rgb (const double gray, vector<double> &rgb) {
	for (int i=0; i < 3; i++)
		rgb[i] = gray;
}


static bool read_doubles (istream &is, vector<double> &v) {
	is.clear();
	for (size_t i=0; i < v.size(); i++) {
		is >> v[i];
		if (is.fail())
			return false;
	}
	return true;
}


static bool read_color (istream &is, vector<double> &rgb) {
	string model;
	is >> model;
	if (model == "rgb")
		return read_doubles(is, rgb);
	if (model == "cmyk") {
		vector<double> cmyk(4);
		if (read_doubles(is, cmyk)) {
			cmyk_to_rgb(cmyk, rgb);
			return true;
		}
	}
	else if (model == "gray") {
		is.clear();
		double v;
		is >> v;
		if (!is.fail()) {
			gray_to_rgb(v, rgb);
			return true;
		}
	}
	return false;
}


bool ColorSpecialHandler::process (istream &is) {
	string cmd;
	is >> cmd;
	vector<double> rgb(3);
	if (cmd == "push" && read_color(is, rgb))       // color push <model> <params>
		_colorStack.push(rgb);
	else if (cmd == "pop" && !_colorStack.empty())  // color pop
		_colorStack.pop();
	else if (read_color(is, rgb)) {                 // color <model> <params>
		while (!_colorStack.empty())
			_colorStack.pop();
		_colorStack.push(rgb);
	}
	else
		return false;
	return true;
}
