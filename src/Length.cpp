/*************************************************************************
** Length.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <sstream>
#include "InputReader.h"
#include "Length.h"

#define UNIT(c1,c2) ((c1 << 8)|c2)

using namespace std;


void Length::set (const string &lenstr) {
	switch (lenstr.length()) {
		case 0:
			_pt = 0;
			break;
		case 1:
			if (isdigit(lenstr[0]))
				_pt = lenstr[0] - '0';
			else
				throw UnitException(string("invalid length: ")+lenstr);
			break;
		default:
			istringstream iss(lenstr);
			StreamInputReader ir(iss);
			double val;
			if (!ir.parseDouble(val))
				throw UnitException(string("invalid length: ")+lenstr);
			string unit = ir.getWord();
			set(val, unit);
	}
}


void Length::set (double val, Unit unit) {
	switch (unit) {
		case PT: _pt = val; break;
		case BP: _pt = val*72.27/72; break;
		case IN: _pt = val*72.27; break;
		case CM: _pt = val/2.54*72.27; break;
		case MM: _pt = val/25.4*72.27; break;
		case PC: _pt = val/12*72.27; break;
		default:
			// this isn't supposed to happen
			ostringstream oss;
			oss << "invalid length unit: (" << unit << ")";
			throw UnitException(oss.str());
	}
}


double Length::get (Unit unit) const {
	switch (unit) {
		case PT: return pt();
		case BP: return bp();
		case IN: return in();
		case CM: return cm();
		case MM: return mm();
		case PC: return pc();
	}
	// this isn't supposed to happen
	ostringstream oss;
	oss << "invalid length unit: (" << unit << ")";
	throw UnitException(oss.str());
}


string Length::toString (Unit unit) const {
	ostringstream oss;
	oss << get(unit) << unitToString(unit);
	return oss.str();
}


Length::Unit Length::unit (const std::string &unitstr) {
	if (unitstr.length() == 2) {
		switch (UNIT(unitstr[0], unitstr[1])) {
			case UNIT('p','t'): return PT;
			case UNIT('b','p'): return BP;
			case UNIT('i','n'): return IN;
			case UNIT('c','m'): return CM;
			case UNIT('m','m'): return MM;
			case UNIT('p','c'): return PC;
		}
	}
	throw UnitException(string("invalid length unit: ")+unitstr);
}


string Length::unitToString (Unit unit) {
	switch (unit) {
		case PT: return "pt";
		case BP: return "bp";
		case IN: return "in";
		case CM: return "cm";
		case MM: return "mm";
		case PC: return "pc";
	}
	// this isn't supposed to happen
	return "??";
}

void Length::set (double val, string unitstr) {
	if (unitstr.empty())
		unitstr = "pt";
	else if (unitstr.length() != 2)
		throw UnitException(string("invalid length unit: ")+unitstr);
	set(val, unit(unitstr));
}

