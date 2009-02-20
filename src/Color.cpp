#include <cmath>
#include <iomanip>
#include <sstream>
#include "Color.h"

#include "debug.h"
using namespace std;

static inline UInt8 float_to_byte (float v) {
	return floor(255*v+0.5);
}


void Color::set (float r, float g, float b) {
	set(float_to_byte(r), float_to_byte(g), float_to_byte(b));
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

