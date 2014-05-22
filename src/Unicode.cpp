/*************************************************************************
** Unicode.cpp                                                          **
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

#include <cstddef>
#include "Unicode.h"

using namespace std;


/** Returns true if c is a valid unicode point in XML documents.
 *  XML version 1.0 doesn't allow various unicode character references
 *  (&#1; for example). */
bool Unicode::isValidCodepoint (UInt32 c) {
	if ((c & 0xffff) == 0xfffe || (c & 0xffff) == 0xffff)
		return false;

	UInt32 ranges[] = {
		0x0000, 0x0020,
		0x007f, 0x0084,
		0x0086, 0x009f,
		0x202a, 0x202e,  // bidi control characters
		0xd800, 0xdfff,
		0xfdd0, 0xfdef,
	};
	for (size_t i=0; i < sizeof(ranges)/sizeof(UInt32)/2; i++)
		if (c >= ranges[2*i] && c <= ranges[2*i+1])
			return false;
	return true;
}


/** Converts a unicode value to a UTF-8 byte sequence.
 *  @param[in] c character code
 *  @return  utf8 seqence consisting of 1-4 bytes */
string Unicode::utf8 (Int32 c) {
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