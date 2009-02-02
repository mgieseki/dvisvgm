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
#include <cmath>
#include <vector>
#include "ColorSpecialHandler.h"

using namespace std;


/** Approximates a CMYK color by an RGB triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] rgb  RGB approximation */
static void cmyk_to_rgb (const vector<float> &cmyk, vector<float> &rgb) {
	const float factor = (cmyk[3] > 0) ? -1 : 1;
	for (int i=0; i < 3; i++) {
		rgb[i] = 1.0-cmyk[i]*factor;
		if (rgb[i] < 0)
			rgb[i] = 0;
		else if (rgb[i] > 1)
			rgb[i] = 1;
	}
}


/** Converts a color given in HSB coordinates to RGB. 
 *  @param[in]  hsb color in HSB space
 *  @param[out] rgb color in RGB space */
static void hsb_to_rgb (const vector<float> &hsb, vector<float> &rgb) {
	if (hsb[1] == 0) 
		rgb[0] = rgb[1] = rgb[2] = hsb[2];
	else {
		float h = hsb[0]-floor(hsb[0]);
		int i = 6*h;
		float f =  6*h-i;
		float p = hsb[2]*(1-hsb[1]);
		float q = hsb[2]*(1-hsb[1]*f);
		float t = hsb[2]*(1-hsb[1]*(1-f));
		switch (i) {
			case 0 : rgb[0]=hsb[2]; rgb[1]=t; rgb[2]=p; break;
			case 1 : rgb[0]=q; rgb[1]=hsb[2]; rgb[2]=p; break;
			case 2 : rgb[0]=p; rgb[1]=hsb[2]; rgb[2]=t; break;
			case 3 : rgb[0]=p; rgb[1]=q; rgb[2]=hsb[2]; break;
			case 4 : rgb[0]=t; rgb[1]=p; rgb[2]=hsb[2]; break;
			case 5 : rgb[0]=hsb[2]; rgb[1]=p; rgb[2]=q; break;
			default: ;  // prevent compiler warning
		}
	}
}


/** Converts a gray value to RGB. 
 *  @param[in]  gray normalized gray value (0 <= gray <= 1) 
 *  @param[out] rgb resulting RGB triple */
static void gray_to_rgb (const float gray, vector<float> &rgb) {
	for (int i=0; i < 3; i++)
		rgb[i] = gray;
}


/** Reads multiple float values from a given stream. The number of 
 *  values read is determined by the size of the result vector.
 *  @param[in]  is stream to be read from 
 *  @param[out] v  the resulting floats */
static bool read_floats (istream &is, vector<float> &v) {
	is.clear();
	for (size_t i=0; i < v.size(); i++) {
		is >> v[i];
		if (is.fail())
			return false;
	}
	return true;
}


/** Reads a color statement from an input stream and converts it to RGB.
 *  A color statement has the following syntax:
 *  <color model> <component values>
 *  Currently, the following color models are supported: rgb, cmyk, hsb and gray.
 *  Examples: rgb 1 0.5 0, gray 0.5 
 *  @param[in]  model if model != "" this value specifies the model, otherwise it's read from the stream
 *  @param[in]  is stream to be read from
 *  @param[out] resulting RGB triple 
 *  @return true if statement has successfully been read */
static bool read_color (string model, istream &is, vector<float> &rgb) {
	if (model.empty())
		is >> model;
	if (model == "rgb")
		return read_floats(is, rgb);
	if (model == "cmyk") {
		vector<float> cmyk(4);
		if (read_floats(is, cmyk)) {
			cmyk_to_rgb(cmyk, rgb);
			return true;
		}
	}
	else if (model == "hsb") {
		vector<float> hsb(3);
		if (read_floats(is, hsb)) {
			hsb_to_rgb(hsb, rgb);
			return true;
		}
	}
	else if (model == "gray") {
		is.clear();
		float v;
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
	vector<float> rgb(3);
	if (cmd == "push" && read_color("", is, rgb))   // color push <model> <params>
		_colorStack.push(rgb);
	else if (cmd == "pop" && !_colorStack.empty())  // color pop
		_colorStack.pop();
	else if (read_color(cmd, is, rgb)) {            // color <model> <params>
		while (!_colorStack.empty())
			_colorStack.pop();
		_colorStack.push(rgb);
	}
	else
		return false;
	return true;
}

