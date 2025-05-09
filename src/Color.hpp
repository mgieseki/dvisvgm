/*************************************************************************
** Color.hpp                                                            **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>
#include <string>
#include <valarray>
#include <vector>
#include "VectorIterator.hpp"

#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

class Color {
	public:
		static bool SUPPRESS_COLOR_NAMES;
		static const Color BLACK;
		static const Color WHITE;

		enum class ColorSpace {GRAY, RGB, CMYK, LAB, TRANSPARENT};

	public:
		Color () noexcept =default;
		explicit Color (uint32_t rgb) noexcept : _value(rgb) {}
		Color (uint32_t value, ColorSpace cs) : _value(value), _cspace(cs) {}
		Color (uint8_t r, uint8_t g, uint8_t b) noexcept  {setRGB(r,g,b);}
		Color (double r, double g, double b) noexcept     {setRGB(r,g,b);}
		explicit Color (const std::valarray<double> &rgb) noexcept {setRGB(rgb);}
		explicit Color (const std::valarray<double> &rgb, ColorSpace cs) noexcept;
		explicit Color (const std::string &colorstr);
		explicit operator uint32_t () const            {return _value;}
		bool operator == (const Color &c) const        {return _value == c._value;}
		bool operator != (const Color &c) const        {return _value != c._value;}
		bool operator < (const Color &c) const         {return _value < c._value;}
		Color operator *= (double c);
		Color operator * (double c) const              {return Color(*this) *= c;}
		bool isTransparent () const                    {return _cspace == ColorSpace::TRANSPARENT;}
		void setRGB (uint8_t r, uint8_t g, uint8_t b);
		void setRGB (double r, double g, double b);
		void setRGB (const std::valarray<double> &rgb) {setRGB(rgb[0], rgb[1], rgb[2]);}
		bool setRGBHexString (std::string hexString);
		bool setPSName (std::string name, bool case_sensitive=true);
		void setGray (uint8_t g)  {setRGB(g,g,g);}
		void setGray (double g) {setRGB(g,g,g);}
		void setGray (const std::valarray<double> &gray) {setRGB(gray[0], gray[0], gray[0]);}
		void setHSB (double h, double s, double b);
		void setCMYK (uint8_t c, uint8_t m, uint8_t y, uint8_t k);
		void setCMYK (double c, double m, double y, double k);
		void setCMYK (const std::valarray<double> &cmyk);
		void setXYZ (double x, double y, double z);
		void setXYZ (const std::valarray<double> &xyz);
		void setLab (double l, double a, double b);
		void setLab (const std::valarray<double> &lab);
		void set (ColorSpace colorSpace, VectorIterator<double> &it);
		std::valarray<double> getDoubleValues () const;
		double getGray () const;
		void getGray (std::valarray<double> &gray) const;
		void getRGB (double &r, double &g, double &b) const;
		void getRGB (std::valarray<double> &rgb) const;
		void getXYZ (double &x, double &y, double &z) const;
		void getLab (double &l, double &a, double &b) const;
		void getLab (std::valarray<double> &lab) const;
		double deltaE (const Color &c) const;
		std::string rgbString () const;
		std::string svgColorString (bool rgbonly) const;
		std::string svgColorString () const {return svgColorString(SUPPRESS_COLOR_NAMES);}
		static void CMYK2RGB (const std::valarray<double> &cmyk, std::valarray<double> &rgb);
		static void HSB2RGB (const std::valarray<double> &hsb, std::valarray<double> &rgb);
		static void RGB2XYZ (std::valarray<double> rgb, std::valarray<double> &xyz);
		static void XYZ2RGB (const std::valarray<double> &xyz, std::valarray<double> &rgb);
		static void RGB2Lab (const std::valarray<double> &rgb, std::valarray<double> &lab);
		static void Lab2XYZ (const std::valarray<double> &lab, std::valarray<double> &xyz);
		static int numComponents (ColorSpace colorSpace);

	protected:
		uint32_t getRGBUInt32 () const;
		std::valarray<double> getRGBDouble () const;

	private:
		uint32_t _value=0;
		ColorSpace _cspace=ColorSpace::RGB;
};

#endif
