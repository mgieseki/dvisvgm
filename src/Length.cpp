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
		case Unit::PT: _pt = val; break;
		case Unit::BP: _pt = val/pt2bp; break;
		case Unit::IN: _pt = val/pt2in; break;
		case Unit::CM: _pt = val/pt2cm; break;
		case Unit::MM: _pt = val/pt2mm; break;
		case Unit::PC: _pt = val/pt2pc; break;
		case Unit::DD: _pt = val/pt2dd; break;
		case Unit::CC: _pt = val/pt2cc; break;
		case Unit::SP: _pt = val/pt2sp; break;
		default:
			// this isn't supposed to happen
			ostringstream oss;
			oss << "invalid length unit: (" << static_cast<int>(unit) << ")";
			throw UnitException(oss.str());
	}
}


double Length::get (Unit unit) const {
	switch (unit) {
		case Unit::PT: return pt();
		case Unit::BP: return bp();
		case Unit::IN: return in();
		case Unit::CM: return cm();
		case Unit::MM: return mm();
		case Unit::PC: return pc();
		case Unit::DD: return dd();
		case Unit::CC: return cc();
		case Unit::SP: return sp();
	}
	// this isn't supposed to happen
	ostringstream oss;
	oss << "invalid length unit: (" << static_cast<int>(unit) << ")";
	throw UnitException(oss.str());
}


string Length::toString (Unit unit) const {
	ostringstream oss;
	oss << get(unit) << unitToString(unit);
	return oss.str();
}


#define UNIT(c1, c2) ((c1 << 8)|c2)

Length::Unit Length::stringToUnit (const std::string &unitstr) {
	if (unitstr.length() == 2) {
		switch (UNIT(unitstr[0], unitstr[1])) {
			case UNIT('p','t'): return Unit::PT;
			case UNIT('b','p'): return Unit::BP;
			case UNIT('i','n'): return Unit::IN;
			case UNIT('c','m'): return Unit::CM;
			case UNIT('m','m'): return Unit::MM;
			case UNIT('p','c'): return Unit::PC;
			case UNIT('d','d'): return Unit::DD;
			case UNIT('c','c'): return Unit::CC;
			case UNIT('s','p'): return Unit::SP;
		}
	}
	throw UnitException(string("invalid length unit: ")+unitstr);
}


string Length::unitToString (Unit unit) {
	switch (unit) {
		case Unit::PT: return "pt";
		case Unit::BP: return "bp";
		case Unit::IN: return "in";
		case Unit::CM: return "cm";
		case Unit::MM: return "mm";
		case Unit::PC: return "pc";
		case Unit::DD: return "dd";
		case Unit::CC: return "cc";
		case Unit::SP: return "sp";
	}
	// this isn't supposed to happen
	return "??";
}


void Length::set (double val, string unitstr) {
	if (unitstr.empty())
		unitstr = "pt";
	else if (unitstr.length() != 2)
		throw UnitException(string("invalid length unit: ")+unitstr);
	set(val, stringToUnit(unitstr));
}
