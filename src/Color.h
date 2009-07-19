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
		void operator *= (double c);
		std::string rgbString () const;
		static void CMYK2RGB (const std::vector<float> &cmyk, std::vector<float> &rgb);
		static void HSB2RGB (const std::vector<float> &hsb, std::vector<float> &rgb);

	private:
		UInt32 _rgb;
};

#endif
