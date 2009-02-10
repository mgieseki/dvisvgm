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
#include <cstring>
#include <sstream>
#include <vector>
#include "ColorSpecialHandler.h"

using namespace std;


/** Approximates a CMYK color by an RGB triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] c  RGB approximation */
static void cmyk_to_rgb (const vector<float> &cmyk, Color &c) {
	c.setRGB(1.0f-min(1.0f, cmyk[0]+cmyk[3]),	1.0f-min(1.0f, cmyk[1]+cmyk[3]),	1.0f-min(1.0f, cmyk[2]+cmyk[3]));
}


/** Converts a color given in HSB coordinates to RGB. 
 *  @param[in]  hsb color in HSB space
 *  @param[out] c   color in RGB space */
static void hsb_to_rgb (const vector<float> &hsb, Color &c) {
	if (hsb[1] == 0)
	  	c.setRGB(hsb[2], hsb[2], hsb[2]);	
	else {
		float h = hsb[0]-floor(hsb[0]);
		int i = 6*h;
		float f =  6*h-i;
		float p = hsb[2]*(1-hsb[1]);
		float q = hsb[2]*(1-hsb[1]*f);
		float t = hsb[2]*(1-hsb[1]*(1-f));
		switch (i) {
			case 0 : c.setRGB(hsb[2], t, p); break;
			case 1 : c.setRGB(q, hsb[2], p); break;
			case 2 : c.setRGB(p, hsb[2], t); break;
			case 3 : c.setRGB(p, q, hsb[2]); break;
			case 4 : c.setRGB(t, p, hsb[2]); break;
			case 5 : c.setRGB(hsb[2], p, q); break;
			default: ;  // prevent compiler warning
		}
	}
}


/** Converts a gray value to RGB. 
 *  @param[in]  gray normalized gray value (0 <= gray <= 1) 
 *  @param[out] c    resulting RGB triple */
static void gray_to_rgb (const float gray, Color &c) {
	c.setRGB(gray, gray, gray);
}


static float read_float (istream &is) {
	is.clear();
	float v;
	is >> v;
	if (is.fail())
		throw SpecialException("number expected");
	return v;
}


/** Reads multiple float values from a given stream. The number of 
 *  values read is determined by the size of the result vector.
 *  @param[in]  is stream to be read from 
 *  @param[out] v  the resulting floats */
static void read_floats (istream &is, vector<float> &v) {
	for (size_t i=0; i < v.size(); i++)
		v[i] = read_float(is);
}


static bool color_constant (const string &name, Color &c) {
	// converted color constants from color.pro
	const struct {
		const char *name;
		const float rgb[3];
	} 
	constants[] = {
		{"Apricot", {1, 0.68, 0.48}},
		{"Aquamarine", {0.18, 1, 0.7}},
		{"Bittersweet", {0.76, 0.01, 0}},
		{"Black", {0, 0, 0}},
		{"Blue", {0, 0, 1}},
		{"BlueGreen", {0.15, 1, 0.67}},
		{"BlueViolet", {0.1, 0.05, 0.96}},
		{"BrickRed", {0.72, 0, 0}},
		{"Brown", {0.4, 0, 0}},
		{"BurntOrange", {1, 0.49, 0}},
		{"CadetBlue", {0.38, 0.43, 0.77}},
		{"CarnationPink", {1, 0.37, 1}},
		{"Cerulean", {0.06, 0.89, 1}},
		{"CornflowerBlue", {0.35, 0.87, 1}},
		{"Cyan", {0, 1, 1}},
		{"Dandelion", {1, 0.71, 0.16}},
		{"DarkOrchid", {0.6, 0.2, 0.8}},
		{"Emerald", {0, 1, 0.5}},
		{"ForestGreen", {0, 0.88, 0}},
		{"Fuchsia", {0.45, 0.01, 0.92}},
		{"Goldenrod", {1, 0.9, 0.16}},
		{"Gray", {0.5, 0.5, 0.5}},
		{"Green", {0, 1, 0}},
		{"GreenYellow", {0.85, 1, 0.31}},
		{"JungleGreen", {0.01, 1, 0.48}},
		{"Lavender", {1, 0.52, 1}},
		{"LimeGreen", {0.5, 1, 0}},
		{"Magenta", {1, 0, 1}},
		{"Mahogany", {0.65, 0, 0}},
		{"Maroon", {0.68, 0, 0}},
		{"Melon", {1, 0.54, 0.5}},
		{"MidnightBlue", {0, 0.44, 0.57}},
		{"Mulberry", {0.64, 0.08, 0.98}},
		{"NavyBlue", {0.06, 0.46, 1}},
		{"OliveGreen", {0, 0.6, 0}},
		{"Orange", {1, 0.39, 0.13}},
		{"OrangeRed", {1, 0, 0.5}},
		{"Orchid", {0.68, 0.36, 1}},
		{"Peach", {1, 0.5, 0.3}},
		{"Periwinkle", {0.43, 0.45, 1}},
		{"PineGreen", {0, 0.75, 0.16}},
		{"Plum", {0.5, 0, 1}},
		{"ProcessBlue", {0.04, 1, 1}},
		{"Purple", {0.55, 0.14, 1}},
		{"RawSienna", {0.55, 0, 0}},
		{"Red", {1, 0, 0}},
		{"RedOrange", {1, 0.23, 0.13}},
		{"RedViolet", {0.59, 0, 0.66}},
		{"Rhodamine", {1, 0.18, 1}},
		{"RoyalBlue", {0, 0.5, 1}},
		{"RoyalPurple", {0.25, 0.1, 1}},
		{"RubineRed", {1, 0, 0.87}},
		{"Salmon", {1, 0.47, 0.62}},
		{"SeaGreen", {0.31, 1, 0.5}},
		{"Sepia", {0.3, 0, 0}},
		{"SkyBlue", {0.38, 1, 0.88}},
		{"SpringGreen", {0.74, 1, 0.24}},
		{"Tan", {0.86, 0.58, 0.44}},
		{"TealBlue", {0.12, 0.98, 0.64}},
		{"Thistle", {0.88, 0.41, 1}},
		{"Turquoise", {0.15, 1, 0.8}},
		{"Violet", {0.21, 0.12, 1}},
		{"VioletRed", {1, 0.19, 1}},
		{"White", {1, 1, 1}},
		{"WildStrawberry", {1, 0.04, 0.61}},
		{"Yellow", {1, 1, 0}},
		{"YellowGreen", {0.56, 1, 0.26}},
		{"YellowOrange", {1, 0.58, 0}}
	};
	// binary search
	int first=0, last=68-1;
	while (first <= last) {
		int mid = first+(last-first)/2;
		int cmp = strcmp(constants[mid].name, name.c_str());
		if (cmp > 0)
			last = mid-1;
		else if (cmp < 0)
			first = mid+1;
		else {
			c.setRGB(constants[mid].rgb[0], constants[mid].rgb[1], constants[mid].rgb[2]);
			return true;
		}			
	}
	return false;
}


/** Reads a color statement from an input stream and converts it to RGB.
 *  A color statement has the following syntax:
 *  <color model> <component values>
 *  Currently, the following color models are supported: rgb, cmyk, hsb and gray.
 *  Examples: rgb 1 0.5 0, gray 0.5 
 *  @param[in]  model if model != "" this value specifies the model, otherwise it's read from the stream
 *  @param[in]  is stream to be read from
 *  @param[out] c  resulting RGB triple 
 *  @return true if statement has successfully been read */
static void read_color (string model, istream &is, Color &c) {
	if (model.empty())
		is >> model;
	if (model == "rgb") {
		vector<float> rgb(3);
		read_floats(is, rgb);
		c.setRGB(rgb[0], rgb[1], rgb[2]);
	}
	else if (model == "cmyk") {
		vector<float> cmyk(4);
		read_floats(is, cmyk);
		cmyk_to_rgb(cmyk, c);
	}
	else if (model == "hsb") {
		vector<float> hsb(3);
		read_floats(is, hsb);
		hsb_to_rgb(hsb, c);
	}
	else if (model == "gray") 
		gray_to_rgb(read_float(is), c);
	else if (!color_constant(model, c))
		throw SpecialException("unknown color statement");
}


void ColorSpecialHandler::process (istream &is) {
	string cmd;
	is >> cmd;
	Color color;
	if (cmd == "push") {            // color push <model> <params>
		read_color("", is, color); 		
		_colorStack.push(color);
	}
	else if (cmd == "pop") {
		if (!_colorStack.empty())  // color pop
		_colorStack.pop();
	}
	else {                         // color <model> <params>
		read_color(cmd, is, color);
		while (!_colorStack.empty())
			_colorStack.pop();
		_colorStack.push(color);
	}
}

