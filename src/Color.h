#ifndef COLOR_H
#define COLOR_H

#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

template <typename T>
class ColorTemplate
{
	static const int SIZE = 3*sizeof(T);
	public:
		ColorTemplate ()                             {_rgb[0]=_rgb[1]=_rgb[2]=0;}
		ColorTemplate (const T *rgb)                 {setRGB(rgb);}
		ColorTemplate (const std::vector<T> &v)      {setRGB(v);}
		ColorTemplate (T r, T g, T b)                {setRGB(r, g, b);}
		ColorTemplate (const ColorTemplate &c)       {setRGB(c._rgb);}
		void operator = (const ColorTemplate<T> &c)  {setRGB(c._rgb);}
//		void operator = (const std::vector<T> &v)    {setRGB(v);}
		bool operator == (const ColorTemplate<T> &c) const  {return std::memcmp(_rgb, c._rgb, SIZE) == 0;}
		bool operator != (const ColorTemplate<T> &c) const  {return std::memcmp(_rgb, c._rgb, SIZE) != 0;}
	   T*& getRGB () const                          {return _rgb;}
		void setRGB (const T *rgb)                   {std::memcpy(_rgb, rgb, SIZE);}
		void setRGB (T r, T g, T b)                  {_rgb[0]=r; _rgb[1]=g; _rgb[2]=b;}
		void setRGB (const std::vector<T> &v)        {_rgb[0]=v[0]; _rgb[1]=v[1]; _rgb[2]=v[2];}
		std::string css () const; 
		
	private:
		T _rgb[3];
};

template <typename T>
std::string ColorTemplate<T>::css () const {
	std::ostringstream oss;
	oss << '#';
	for (int i=0; i < 3; i++)
		oss << std::setbase(16) << std::setfill('0') << std::setw(2) << (int)std::floor(255*_rgb[i]+0.5);
	return oss.str();
}

typedef ColorTemplate<float> Color;

#endif
