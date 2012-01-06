/*************************************************************************
** ColorSpecialHandler.cpp                                              **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>
#include "ColorSpecialHandler.h"
#include "SpecialActions.h"

using namespace std;



/** Approximates a CMYK color by an RGB triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] rgb  RGB approximation */
static void cmyk_to_rgb (const vector<float> &cmyk, vector<float> &rgb) {
	for (int i=0; i < 3; i++)
		rgb[i] = 1.0f-min(1.0f, cmyk[i]+cmyk[3]);
}


/** Converts a color given in HSB coordinates to RGB.
 *  @param[in]  hsb color in HSB space
 *  @param[out] rgb color in RGB space */
static void hsb_to_rgb (const vector<float> &hsb, vector<float> &rgb) {
	if (hsb[1] == 0)
		rgb[0] = rgb[1] = rgb[2] = hsb[2];
	else {
		float h = hsb[0]-floor(hsb[0]);
		float i = floor(6*h);
		float f = 6.0f*h-i;
		float p = hsb[2]*(1-hsb[1]);
		float q = hsb[2]*(1-hsb[1]*f);
		float t = hsb[2]*(1-hsb[1]*(1-f));
		switch (int(i)) {
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


static bool color_constant (const string &c, vector<float> &rgb) {
	// converted color constants from color.pro
	const struct {
		const char *name;
		const float rgb[3];
	}
	constants[] = {
		{"Apricot", {1, 0.68f, 0.48f}},
		{"Aquamarine", {0.18f, 1, 0.7f}},
		{"Bittersweet", {0.76f, 0.01f, 0}},
		{"Black", {0, 0, 0}},
		{"Blue", {0, 0, 1}},
		{"BlueGreen", {0.15f, 1, 0.67f}},
		{"BlueViolet", {0.1f, 0.05f, 0.96f}},
		{"BrickRed", {0.72f, 0, 0}},
		{"Brown", {0.4f, 0, 0}},
		{"BurntOrange", {1, 0.49f, 0}},
		{"CadetBlue", {0.38f, 0.43f, 0.77f}},
		{"CarnationPink", {1, 0.37f, 1}},
		{"Cerulean", {0.06f, 0.89f, 1}},
		{"CornflowerBlue", {0.35f, 0.87f, 1}},
		{"Cyan", {0, 1, 1}},
		{"Dandelion", {1, 0.71f, 0.16f}},
		{"DarkOrchid", {0.6f, 0.2f, 0.8f}},
		{"Emerald", {0, 1, 0.5f}},
		{"ForestGreen", {0, 0.88f, 0}},
		{"Fuchsia", {0.45f, 0.01f, 0.92f}},
		{"Goldenrod", {1, 0.9f, 0.16f}},
		{"Gray", {0.5f, 0.5f, 0.5f}},
		{"Green", {0, 1, 0}},
		{"GreenYellow", {0.85f, 1, 0.31f}},
		{"JungleGreen", {0.01f, 1, 0.48f}},
		{"Lavender", {1, 0.52f, 1}},
		{"LimeGreen", {0.5f, 1, 0}},
		{"Magenta", {1, 0, 1}},
		{"Mahogany", {0.65f, 0, 0}},
		{"Maroon", {0.68f, 0, 0}},
		{"Melon", {1, 0.54f, 0.5f}},
		{"MidnightBlue", {0, 0.44f, 0.57f}},
		{"Mulberry", {0.64f, 0.08f, 0.98f}},
		{"NavyBlue", {0.06f, 0.46f, 1}},
		{"OliveGreen", {0, 0.6f, 0}},
		{"Orange", {1, 0.39f, 0.13f}},
		{"OrangeRed", {1, 0, 0.5f}},
		{"Orchid", {0.68f, 0.36f, 1}},
		{"Peach", {1, 0.5f, 0.3f}},
		{"Periwinkle", {0.43f, 0.45f, 1}},
		{"PineGreen", {0, 0.75f, 0.16f}},
		{"Plum", {0.5f, 0, 1}},
		{"ProcessBlue", {0.04f, 1, 1}},
		{"Purple", {0.55f, 0.14f, 1}},
		{"RawSienna", {0.55f, 0, 0}},
		{"Red", {1, 0, 0}},
		{"RedOrange", {1, 0.23f, 0.13f}},
		{"RedViolet", {0.59f, 0, 0.66f}},
		{"Rhodamine", {1, 0.18f, 1}},
		{"RoyalBlue", {0, 0.5f, 1}},
		{"RoyalPurple", {0.25f, 0.1f, 1}},
		{"RubineRed", {1, 0, 0.87f}},
		{"Salmon", {1, 0.47f, 0.62f}},
		{"SeaGreen", {0.31f, 1, 0.5f}},
		{"Sepia", {0.3f, 0, 0}},
		{"SkyBlue", {0.38f, 1, 0.88f}},
		{"SpringGreen", {0.74f, 1, 0.24f}},
		{"Tan", {0.86f, 0.58f, 0.44f}},
		{"TealBlue", {0.12f, 0.98f, 0.64f}},
		{"Thistle", {0.88f, 0.41f, 1}},
		{"Turquoise", {0.15f, 1, 0.8f}},
		{"Violet", {0.21f, 0.12f, 1}},
		{"VioletRed", {1, 0.19f, 1}},
		{"White", {1, 1, 1}},
		{"WildStrawberry", {1, 0.04f, 0.61f}},
		{"Yellow", {1, 1, 0}},
		{"YellowGreen", {0.56f, 1, 0.26f}},
		{"YellowOrange", {1, 0.58f, 0}}
	};
	// binary search
	int first=0, last=68-1;
	while (first <= last) {
		int mid = first+(last-first)/2;
		int cmp = strcmp(constants[mid].name, c.c_str());
		if (cmp > 0)
			last = mid-1;
		else if (cmp < 0)
			first = mid+1;
		else {
			rgb[0] = constants[mid].rgb[0];
			rgb[1] = constants[mid].rgb[1];
			rgb[2] = constants[mid].rgb[2];
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
 *  @param[out] resulting RGB triple
 *  @return true if statement has successfully been read */
static void read_color (string model, istream &is, vector<float> &rgb) {
	if (model.empty())
		is >> model;
	if (model == "rgb")
		read_floats(is, rgb);
	else if (model == "cmyk") {
		vector<float> cmyk(4);
		read_floats(is, cmyk);
		cmyk_to_rgb(cmyk, rgb);
	}
	else if (model == "hsb") {
		vector<float> hsb(3);
		read_floats(is, hsb);
		hsb_to_rgb(hsb, rgb);
	}
	else if (model == "gray")
		gray_to_rgb(read_float(is), rgb);
	else if (!color_constant(model, rgb))
		throw SpecialException("unknown color statement");
	if (rgb[0] > 1 || rgb[1] > 1 || rgb[2] > 1) {
		ostringstream oss;
		oss << "invalid RGB value (" << rgb[0] << ',' << rgb[1] << ',' << rgb[2] << ')';
		throw SpecialException(oss.str());
	}
}


bool ColorSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	vector<float> rgb(3);
	if (prefix && strcmp(prefix, "background") == 0) {
		read_color("", is, rgb);
		actions->setBgColor(rgb);
	}
	else {
		string cmd;
		is >> cmd;
		if (cmd == "push") {             // color push <model> <params>
			read_color("", is, rgb);
			_colorStack.push(rgb);
		}
		else if (cmd == "pop") {
			if (!_colorStack.empty())     // color pop
				_colorStack.pop();
		}
		else {                           // color <model> <params>
			read_color(cmd, is, rgb);
			while (!_colorStack.empty())
				_colorStack.pop();
			_colorStack.push(rgb);
		}
		if (actions) {
		  	if (_colorStack.empty())
				actions->setColor(Color::BLACK);
			else
				actions->setColor(_colorStack.top());
		}
	}
	return true;
}


const char** ColorSpecialHandler::prefixes () const {
	static const char *pfx[] = {"color", 0};
	return pfx;
}

