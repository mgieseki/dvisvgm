/*************************************************************************
** Color.h                                                              **
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

#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <vector>
#include "types.h"

class Color
{
	public:
		static const Color BLACK;
		static const Color WHITE;

	public:
		Color () : _rgb(0) {}
		Color (UInt32 rgb) : _rgb(rgb)        {}
		Color (UInt8 r, UInt8 g, UInt8 b)     {set(r,g,b);}
		Color (float r, float g, float b)     {set(r,g,b);}
		Color (const std::vector<float> &rgb) {set(rgb[0], rgb[1], rgb[2]);}
		operator UInt32 () const              {return _rgb;}
		bool operator == (const Color &c)     {return _rgb == c._rgb;}
		bool operator != (const Color &c)     {return _rgb != c._rgb;}
		void set (UInt8 r, UInt8 g, UInt8 b)  {_rgb = (r << 16) | (g << 8) | b;}
		void set (float r, float g, float b);
		void setGray (UInt8 g)                {set(g,g,g);}
		void setGray (float g)                {set(g,g,g);}
		void setHSB (float h, float s, float b);
		void setCMYK (float c, float m, float y, float k);
		void getRGB (float &r, float &g, float &b) const; 
		void operator *= (double c);
		std::string rgbString () const;
		static void CMYK2RGB (const std::vector<float> &cmyk, std::vector<float> &rgb);
		static void HSB2RGB (const std::vector<float> &hsb, std::vector<float> &rgb);

	private:
		UInt32 _rgb;
};

#endif
