#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <vector>
#include "types.h"

class Color
{
	public:
		Color (UInt32 rgb) : _rgb(rgb) {}
		Color (UInt8 r, UInt8 g, UInt8 b)     {set(r,g,b);}
		Color (float r, float g, float b)     {set(r,g,b);}
		Color (const std::vector<float> &rgb) {set(rgb[0], rgb[1], rgb[2]);}
		operator UInt32 () const              {return _rgb;}
		void set (UInt8 r, UInt8 g, UInt8 b)  {_rgb = (r << 16) | (g << 8) | b;}
		void set (float r, float g, float b);
		void operator *= (double c);
		std::string rgbString () const;

	private:
		UInt32 _rgb;
};

#endif
