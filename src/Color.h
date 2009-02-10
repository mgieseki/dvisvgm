#ifndef COLOR_H
#define COLOR_H

#include <cstring>

template <typename T>
class ColorTemplate
{
	static const int SIZE = 3*sizeof(T);
	public:
		ColorTemplate ()                             {_rgb[0]=_rgb[1]=_rgb[2]=0;}
		ColorTemplate (const T *rgb)                 {setRGB(rgb);}
		ColorTemplate (T r, T g, T b)                {setRGB(r, g, b);}
		ColorTemplate (const ColorTemplate &c)       {setRGB(c._rgb);}
		void operator = (const ColorTemplate<T> &c)  {setRGB(c._rgb);}
		bool operator == (const ColorTemplate<T> &c) {return std::memcmp(_rgb, c._rgb, SIZE) == 0;}
		bool operator != (const ColorTemplate<T> &c) {return std::memcmp(_rgb, c._rgb, SIZE) != 0;}
	   T*& getRGB () const                          {return _rgb;}
		void setRGB (const T *rgb)                   {std::memcpy(_rgb, rgb, SIZE);}
		void setRGB (T r, T g, T b)                  {_rgb[0]=r; _rgb[1]=g; _rgb[2]=b;}
		
	private:
		T _rgb[3];
};

typedef ColorTemplate<float> Color;

#endif
