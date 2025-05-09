/*************************************************************************
** JFM.cpp                                                              **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <fstream>
#include <sstream>
#include "JFM.hpp"
#include "StreamReader.hpp"

using namespace std;


void JFM::read (istream &is) {
	is.seekg(0);
	StreamReader reader(is);
	auto id = uint16_t(reader.readUnsigned(2)); // JFM ID (9 or 11)
	if (id != 9 && id != 11)
		throw FontMetricException("invalid JFM identifier " + std::to_string(id) + " (9 or 11 expected)");

	_vertical = (id == 9);
	auto nt = uint16_t(reader.readUnsigned(2));  // length of character type table
	auto lf = uint16_t(reader.readUnsigned(2));  // length of entire file in 4 byte words
	auto lh = uint16_t(reader.readUnsigned(2));  // length of header in 4 byte words
	auto bc = uint16_t(reader.readUnsigned(2));  // smallest character code in font
	auto ec = uint16_t(reader.readUnsigned(2));  // largest character code in font
	auto nw = uint16_t(reader.readUnsigned(2));  // number of words in width table
	auto nh = uint16_t(reader.readUnsigned(2));  // number of words in height table
	auto nd = uint16_t(reader.readUnsigned(2));  // number of words in depth table
	auto ni = uint16_t(reader.readUnsigned(2));  // number of words in italic corr. table
	auto nl = uint16_t(reader.readUnsigned(2));  // number of words in glue/kern table
	auto nk = uint16_t(reader.readUnsigned(2));  // number of words in kern table
	auto ng = uint16_t(reader.readUnsigned(2));  // number of words in glue table
	auto np = uint16_t(reader.readUnsigned(2));  // number of font parameter words

	if (7+nt+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ng+np != lf)
		throw FontMetricException("inconsistent length values");

	setCharRange(bc, ec);
	readHeader(reader);
	is.seekg(28+lh*4);
	readTables(reader, nt, nw, nh, nd, ni);
	is.seekg(4*(lf-np));
	readParameters(reader, np);   // JFM files provide 9 parameters but we don't need all of them
}


void JFM::readTables (StreamReader &reader, int nt, int nw, int nh, int nd, int ni) {
	// determine smallest charcode with chartype > 0
	uint32_t minchar=0xFFFFFFFF, maxchar=0;
	for (int i=0; i < nt; i++) {
		// support new JFM spec by texjporg
		uint32_t c = reader.readUnsigned(2);
		c += 0x10000 * reader.readUnsigned(1);
		auto t =  uint8_t(reader.readUnsigned(1));
		if (t > 0) {
			minchar = min(minchar, c);
			maxchar = max(maxchar, c);
		}
	}
	// build charcode to chartype map
	if (minchar <= maxchar) {
		_minchar = minchar;
		_charTypeTable.resize(maxchar-minchar+1, 0);
		reader.seek(-nt*4, ios::cur);
		for (int i=0; i < nt; i++) {
			// support new JFM spec by texjporg
			uint32_t c = reader.readUnsigned(2);
			c += 0x10000 * reader.readUnsigned(1);
			auto t = uint8_t(reader.readUnsigned(1));
			if (c >= minchar)
				_charTypeTable[c-minchar] = t;
		}
	}
	TFM::readTables(reader, nw, nh, nd, ni);
}


size_t JFM::charIndex (int c) const {
	uint8_t chartype = 0;
	if (!_charTypeTable.empty() && uint32_t(c) >= _minchar && uint32_t(c) < _minchar+_charTypeTable.size())
		chartype = _charTypeTable[c-_minchar];
	return TFM::charIndex(chartype);
}
