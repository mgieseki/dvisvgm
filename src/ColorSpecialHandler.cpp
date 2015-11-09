/*************************************************************************
** ColorSpecialHandler.cpp                                              **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>
#include "ColorSpecialHandler.h"
#include "SpecialActions.h"

using namespace std;


static double read_double (istream &is) {
	is.clear();
	double v;
	is >> v;
	if (is.fail())
		throw SpecialException("number expected");
	return v;
}


/** Reads multiple double values from a given stream. The number of
 *  values read is determined by the size of the result vector.
 *  @param[in]  is stream to be read from
 *  @param[out] v the resulting values */
static void read_doubles (istream &is, vector<double> &v) {
	for (size_t i=0; i < v.size(); i++)
		v[i] = read_double(is);
}


/** Reads a color statement from an input stream and converts it to RGB.
 *  A color statement has the following syntax:
 *  _color model_ _component values_
 *  Currently, the following color models are supported: rgb, cmyk, hsb and gray.
 *  Examples: rgb 1 0.5 0, gray 0.5
 *  @param[in]  model if model != "" this value specifies the model, otherwise it's read from the stream
 *  @param[in]  is stream to be read from
 *  @param[out] color italicresulting RGB triple
 *  @return true if statement has successfully been read */
static void read_color (string model, istream &is, Color &color) {
	if (model.empty())
		is >> model;
	if (model == "rgb") {
		vector<double> rgb(3);
		read_doubles(is, rgb);
		color.setRGB(rgb[0], rgb[1], rgb[2]);
	}
	else if (model == "cmyk") {
		vector<double> cmyk(4);
		read_doubles(is, cmyk);
		color.setCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3]);
	}
	else if (model == "hsb") {
		vector<double> hsb(3);
		read_doubles(is, hsb);
		color.setHSB(hsb[0], hsb[1], hsb[2]);
	}
	else if (model == "gray")
		color.setGray(read_double(is));
	else if (!color.setPSName(model, true))
		throw SpecialException("unknown color statement");
}


bool ColorSpecialHandler::process (const char *prefix, istream &is, SpecialActions *actions) {
	Color color;
	if (prefix && strcmp(prefix, "background") == 0) {
		read_color("", is, color);
		actions->setBgColor(color);
	}
	else {
		string cmd;
		is >> cmd;
		if (cmd == "push") {             // color push <model> <params>
			read_color("", is, color);
			_colorStack.push(color);
		}
		else if (cmd == "pop") {
			if (!_colorStack.empty())     // color pop
				_colorStack.pop();
		}
		else {                           // color <model> <params>
			read_color(cmd, is, color);
			while (!_colorStack.empty())
				_colorStack.pop();
			_colorStack.push(color);
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

