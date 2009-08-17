/*************************************************************************
** StreamReader.cpp                                                     **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "StreamReader.h"
#include "macros.h"

using namespace std;

StreamReader::StreamReader (istream &s) 
	: is(&s)
{
}


istream& StreamReader::replaceStream (istream &in) {
	istream &ret = *is;
	is = &in;
	return ret;
}


/** Reads an unsigned integer from assigned input stream. 
 *  @param[in] bytes number of bytes to read (max. 4)
 *  @return read integer */
UInt32 StreamReader::readUnsigned (int bytes) {
	UInt32 ret = 0;
	for (bytes--; bytes >= 0 && !is->eof(); bytes--) {
		UInt32 b = is->get();
		ret |= b << (8*bytes);
	}
	return ret;
}


/** Reads an signed integer from assigned input stream. 
 *  @param[in] bytes number of bytes to read (max. 4)
 *  @return read integer */
Int32 StreamReader::readSigned (int bytes) {
	Int32 ret = is->get();
	if (ret & 128)        // negative value?
		ret |= 0xffffff00;
	for (bytes-=2; bytes >= 0 && !is->eof(); bytes--) 
		ret = (ret << 8) | is->get();
	return ret;
}


string StreamReader::readString (int length) {
	if (!is)
		throw StreamReaderException("no stream assigned");
	char *buf = new char[length+1];
	if (length > 0)
		is->get(buf, length+1);  // reads 'length' bytes (pos. length+1 is set to 0)
	else
		*buf = 0;
	string ret = buf;
	delete [] buf;
	return ret;
}


vector<UInt8>& StreamReader::readBytes (int n, vector<UInt8> &bytes) {
	if (n > 0)
		in().read((char*)&bytes[0], n);
	return bytes;
}


