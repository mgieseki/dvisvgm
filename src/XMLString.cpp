/*************************************************************************
** XMLString.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "macros.h"
#include "XMLString.h"

using namespace std;

int XMLString::DECIMAL_PLACES = 0;

static string translate (unsigned c) {
	switch (c) {
		case '<' : return "&lt;";
		case '&' : return "&amp;";
		case '"' : return "&quot;";
		case '\'': return "&apos;";
	}
	ostringstream oss;
	if (c >= 32 && c <= 126)
		oss << char(c);
	else
		oss <<"&#" << unsigned(c) << ';';
	return oss.str();
}


#if 0
/** Returns the number of pre-decimal places of a given floating point value. */
static int predecimal_places (double x) {
	int n = abs(static_cast<int>(x));
	int ret = (n == 0 ? 0 : 1);
	while (n >= 10) {
		ret++;
		 n /= 10;
	}
	return ret;
}
#endif


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
