/*************************************************************************
** Color.h                                                              **
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

#ifndef DVISVGM_COLOR_H
#define DVISVGM_COLOR_H

#include <string>
#include <vector>
#include "types.h"

#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

class Color
{
	public:
		static const Color BLACK;
		static const Color WHITE;
		static const Color TRANSPARENT;

	public:
		Color () : _rgb(0) {}
		Color (UInt32 rgb) : _rgb(rgb)          {}
		Color (UInt8 r, UInt8 g, UInt8 b)       {setRGB(r,g,b);}
		Color (double r, double g, double b)    {setRGB(r,g,b);}
		Color (const std::vector<double> &rgb)  {setRGB(rgb[0], rgb[1], rgb[2]);}
		Color (const char *name);
		Color (const std::string &name);
		operator UInt32 () const                {return _rgb;}
		bool operator == (const Color &c) const {return _rgb == c._rgb;}
		bool operator != (const Color &c) const {return _rgb != c._rgb;}
		void setRGB (UInt8 r, UInt8 g, UInt8 b) {_rgb = (r << 16) | (g << 8) | b;}
		void setRGB (double r, double g, double b);
		bool setName (std::string name, bool case_sensitive=true);
		void setGray (UInt8 g)                  {setRGB(g,g,g);}
		void setGray (double g)                 {setRGB(g,g,g);}
		void setHSB (double h, double s, double b);
		void setCMYK (double c, double m, double y, double k);
		void getRGB (double &r, double &g, double &b) const;
		void operator *= (double c);
		std::string rgbString () const;
		static void CMYK2RGB (const std::vector<double> &cmyk, std::vector<double> &rgb);
		static void HSB2RGB (const std::vector<double> &hsb, std::vector<double> &rgb);

	private:
		UInt32 _rgb;
};

#endif
