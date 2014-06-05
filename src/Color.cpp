/*************************************************************************
** Color.cpp                                                            **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "Color.h"

using namespace std;


const Color Color::BLACK(UInt32(0));
const Color Color::WHITE(UInt8(255), UInt8(255), UInt8(255));
const Color Color::TRANSPARENT(UInt32(0xff000000));


static inline UInt8 float_to_byte (float v) {
	v = max(0.0f, min(1.0f, v));
	return UInt8(floor(255*v+0.5));
}


static void tolower (string &str) {
	for (size_t i=0; i < str.length(); i++)
		str[i] = tolower(str[i]);
}


Color::Color (const char *name) {
	if (!set(name, false))
		setGray(UInt8(0));
}


Color::Color (const string &name) {
	if (!set(name, false))
		setGray(UInt8(0));
}


void Color::set (float r, float g, float b) {
	set(float_to_byte(r), float_to_byte(g), float_to_byte(b));
}


bool Color::set (string name, bool case_sensitive) {
	if (name[0] == '#') {
		char *p=0;
		_rgb = UInt32(strtol(name.c_str()+1, &p, 16));
		while (isspace(*p))
			p++;
		return (*p == 0 && _rgb <= 0xFFFFFF);
	}
	// converted color constants from color.pro
	static const struct ColorConstant {
		const char *name;
		const UInt32 rgb;
	}
	constants[] = {
		{"Apricot",        0xFFAD7A},
		{"Aquamarine",     0x2DFFB2},
		{"Bittersweet",    0xC10200},
		{"Black",          0x000000},
		{"Blue",           0x0000FF},
		{"BlueGreen",      0x26FFAA},
		{"BlueViolet",     0x190CF4},
		{"BrickRed",       0xB70000},
		{"Brown",          0x660000},
		{"BurntOrange",    0xFF7C00},
		{"CadetBlue",      0x606DC4},
		{"CarnationPink",  0xFF5EFF},
		{"Cerulean",       0x0FE2FF},
		{"CornflowerBlue", 0x59DDFF},
		{"Cyan",           0x00FFFF},
		{"Dandelion",      0xFFB528},
		{"DarkOrchid",     0x9932CC},
		{"Emerald",        0x00FF7F},
		{"ForestGreen",    0x00E000},
		{"Fuchsia",        0x7202EA},
		{"Goldenrod",      0xFFE528},
		{"Gray",           0x7F7F7F},
		{"Green",          0x00FF00},
		{"GreenYellow",    0xD8FF4F},
		{"JungleGreen",    0x02FF7A},
		{"Lavender",       0xFF84FF},
		{"LimeGreen",      0x7FFF00},
		{"Magenta",        0xFF00FF},
		{"Mahogany",       0xA50000},
		{"Maroon",         0xAD0000},
		{"Melon",          0xFF897F},
		{"MidnightBlue",   0x007091},
		{"Mulberry",       0xA314F9},
		{"NavyBlue",       0x0F75FF},
		{"OliveGreen",     0x009900},
		{"Orange",         0xFF6321},
		{"OrangeRed",      0xFF007F},
		{"Orchid",         0xAD5BFF},
		{"Peach",          0xFF7F4C},
		{"Periwinkle",     0x6D72FF},
		{"PineGreen",      0x00BF28},
		{"Plum",           0x7F00FF},
		{"ProcessBlue",    0x0AFFFF},
		{"Purple",         0x8C23FF},
		{"RawSienna",      0x8C0000},
		{"Red",            0xFF0000},
		{"RedOrange",      0xFF3A21},
		{"RedViolet",      0x9600A8},
		{"Rhodamine",      0xFF2DFF},
		{"RoyalBlue",      0x007FFF},
		{"RoyalPurple",    0x3F19FF},
		{"RubineRed",      0xFF00DD},
		{"Salmon",         0xFF779E},
		{"SeaGreen",       0x4FFF7F},
		{"Sepia",          0x4C0000},
		{"SkyBlue",        0x60FFE0},
		{"SpringGreen",    0xBCFF3D},
		{"Tan",            0xDB9370},
		{"TealBlue",       0x1EF9A3},
		{"Thistle",        0xE068FF},
		{"Turquoise",      0x26FFCC},
		{"Violet",         0x351EFF},
		{"VioletRed",      0xFF30FF},
		{"White",          0xFFFFFF},
		{"WildStrawberry", 0xFF0A9B},
		{"Yellow",         0xFFFF00},
		{"YellowGreen",    0x8EFF42},
		{"YellowOrange",   0xFF9300},
	};
	if (!case_sensitive) {
		tolower(name);
		for (size_t i=0; i < sizeof(constants)/sizeof(ColorConstant); i++) {
			string cmpname = constants[i].name;
			tolower(cmpname);
			if (name == cmpname) {
				_rgb = constants[i].rgb;
				return true;
			}
		}
		return false;
	}

	// binary search
	int first=0, last=sizeof(constants)/sizeof(ColorConstant)-1;
	while (first <= last) {
		int mid = first+(last-first)/2;
		int cmp = strcmp(constants[mid].name, name.c_str());
		if (cmp > 0)
			last = mid-1;
		else if (cmp < 0)
			first = mid+1;
		else {
			_rgb = constants[mid].rgb;
			return true;
		}
	}
	return false;
}


void Color::setHSB (float h, float s, float b) {
	vector<float> hsb(3), rgb(3);
	hsb[0] = h;
	hsb[1] = s;
	hsb[2] = b;
	HSB2RGB(hsb, rgb);
	set(rgb[0], rgb[1], rgb[2]);
}


void Color::setCMYK (float c, float m, float y, float k) {
	vector<float> cmyk(4), rgb(3);
	cmyk[0] = c;
	cmyk[1] = m;
	cmyk[2] = y;
	cmyk[3] = k;
	CMYK2RGB(cmyk, rgb);
	set(rgb[0], rgb[1], rgb[2]);
}


void Color::operator *= (double c) {
	UInt32 rgb=0;
	for (int i=0; i < 3; i++) {
		rgb |= UInt32(floor((_rgb & 0xff)*c+0.5)) << (8*i);
		_rgb >>= 8;
	}
	_rgb = rgb;
}


string Color::rgbString () const {
	ostringstream oss;
	oss << '#';
	for (int i=2; i >= 0; i--) {
		oss << setbase(16) << setfill('0') << setw(2)
			 << (((_rgb >> (8*i)) & 0xff));
	}
	return oss.str();
}


/** Approximates a CMYK color by an RGB triple. The component values
 *  are expected to be normalized, i.e. 0 <= cmyk[i],rgb[j] <= 1.
 *  @param[in]  cmyk color in CMYK space
 *  @param[out] rgb  RGB approximation */
void Color::CMYK2RGB (const vector<float> &cmyk, vector<float> &rgb) {
	for (int i=0; i < 3; i++)
		rgb[i] = 1.0f-max(0.0f, min(1.0f, cmyk[i]+cmyk[3]));
}


/** Converts a color given in HSB coordinates to RGB.
 *  @param[in]  hsb color in HSB space
 *  @param[out] rgb color in RGB space */
void Color::HSB2RGB (const vector<float> &hsb, vector<float> &rgb) {
	if (hsb[1] == 0)
		rgb[0] = rgb[1] = rgb[2] = hsb[2];
	else {
		float h = hsb[0]-floor(hsb[0]);
		int i = int(6*h);
		float f =  float(6.0*h-i);
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


void Color::getRGB (float &r, float &g, float &b) const {
	r = float(((_rgb >> 16) & 255) / 255.0);
	g = float(((_rgb >> 8) & 255) / 255.0);
	b = float((_rgb & 255) / 255.0);
}
