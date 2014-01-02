/*************************************************************************
** XMLString.cpp                                                        **
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
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "macros.h"
#include "types.h"
#include "XMLString.h"

using namespace std;

int XMLString::DECIMAL_PLACES = 0;


/** Converts a unicode value to a UTF-8 byte sequence.
 *  @param[in] c character code
 *  @return  utf8 seqence consisting of 1-4 bytes */
static string to_utf8 (Int32 c) {
	string utf8;
	if (c >= 0) {
		if (c < 0x80)
			utf8 += c;
		else if (c < 0x800) {
			utf8 += 0xC0 + (c >> 6);
			utf8 += 0x80 + (c & 0x3F);
		}
		else if (c == 0xFFFE || c == 0xFFFF)
			utf8 += (c & 0xFF);
		else if (c < 0x10000) {
			utf8 += 0xE0 + (c >> 12);
			utf8 += 0x80 + ((c >> 6) & 0x3F);
			utf8 += 0x80 + (c & 0x3F);
		}
		else if (c < 0x110000) {
			utf8 += 0xF0 + (c >> 18);
			utf8 += 0x80 + ((c >> 12) & 0x3F);
			utf8 += 0x80 + ((c >> 6) & 0x3F);
			utf8 += 0x80 + (c & 0x3F);
		}
	}
	return utf8;
}


static string translate (UInt32 c) {
	switch (c) {
		case '<' : return "&lt;";
		case '&' : return "&amp;";
		case '"' : return "&quot;";
		case '\'': return "&apos;";
	}
	return to_utf8(c);
}


/** Rounds a floating point value to a given number of decimal places.
 *  @param[in] x number to round
 *  @param[in] n number of decimal places (must be between 1 and 6)
 *  @return rounded value */
static inline double round (double x, long n) {
	const long pow10[] = {10L, 100L, 1000L, 10000L, 100000L, 1000000L};
	n--;
   return floor(x*pow10[n]+0.5)/pow10[n];
}


XMLString::XMLString (const string &str, bool plain) {
	if (plain)
		*this = str;
	else {
		FORALL(str, string::const_iterator, i)
			*this += translate(*i);
	}
}


XMLString::XMLString (const char *str, bool plain) {
	if (str) {
		if (plain)
			*this = str;
		else {
			while (*str)
				*this += translate(*str++);
		}
	}
}


XMLString::XMLString (int n, bool cast) {
	if (cast) {
		stringstream ss;
		ss << n;
		ss >> *this;
	}
	else
		*this += translate(n);
}


XMLString::XMLString (double x) {
	stringstream ss;
	if (DECIMAL_PLACES > 0)
		x = round(x, DECIMAL_PLACES);
	else if (fabs(x) < 1e-8)
		x = 0;
	ss << x;
	ss >> *this;
}


/*
ostream& XMLString::write (ostream &os) const {
	const string *self = static_cast<const string*>(this);
	FORALL(*self, string::const_iterator, i) {
		unsigned char c = *i;
		switch (c) {
			case '<' : os << "&lt;"; break;
			case '&' : os << "&amp;"; break;
			case '"' : os << "&quot;"; break;
			case '\'': os << "&apos;"; break;
			default  :
				if (c >= 32 && c <= 126)
					os << c;
				else
					os << "&#" << int(c) << ';';
		}
	}
	return os;
}
*/
