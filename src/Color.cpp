#include <cmath>
#include <iomanip>
#include <sstream>
#include "Color.h"

using namespace std;

const Color Color::BLACK(0);
const Color Color::WHITE(UInt8(255), UInt8(255), UInt8(255));


static inline UInt8 float_to_byte (float v) {
	return floor(255*v+0.5);
}


void Color::set (float r, float g, float b) {
	set(float_to_byte(r), float_to_byte(g), float_to_byte(b));
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

