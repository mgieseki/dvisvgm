/*************************************************************************
** StreamWriter.cpp                                                     **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "StreamWriter.h"

using namespace std;


/** Writes an unsigned integer to the output stream.
 *  @param[in] val the value to write
 *  @param[in] n number of bytes to be considered */
void StreamWriter::writeUnsigned (UInt32 val, int n) {
	for (n--; n >= 0; n--)
		_os.put((val >> (8*n)) & 0xff);
}


/** Writes a signed integer to the output stream.
 *  @param[in] val the value to write
 *  @param[in] n number of bytes to be considered */
void StreamWriter::writeSigned (Int32 val, int n) {
	writeUnsigned((UInt32)val, n);
}


/** Writes a signed integer to the output stream.
 *  @param[in] val the value to write
 *  @param[in] finalZero if true, a final 0-byte is appended */
void StreamWriter::writeString (const string &str, bool finalZero) {
	for (size_t i=0; i < str.length(); i++)
		_os.put(str[i]);
	if (finalZero)
		_os.put(0);
}
