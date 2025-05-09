/*************************************************************************
** TFM.cpp                                                              **
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
#include <vector>
#include "algorithm.hpp"
#include "Length.hpp"
#include "Message.hpp"
#include "StreamReader.hpp"
#include "TFM.hpp"

using namespace std;


/** Reads a sequence of n TFM words (4 Bytes each).
 *  @param[in]  reader the TFM data is read from this object
 *  @param[out] v the read words
 *  @param[in]  n number of words to be read */
template <typename T>
static void read_words (StreamReader &reader, vector<T> &v, unsigned n) {
	v.resize(n);
	for (unsigned i=0; i < n; i++)
		v[i] = reader.readUnsigned(4);
}


void TFM::read (istream &is) {
	if (!is)
		return;
	is.seekg(0);
	StreamReader reader(is);
	auto lf = uint16_t(reader.readUnsigned(2)); // length of entire file in 4 byte words
	auto lh = uint16_t(reader.readUnsigned(2)); // length of header in 4 byte words
	auto bc = uint16_t(reader.readUnsigned(2)); // smallest character code in font
	auto ec = uint16_t(reader.readUnsigned(2)); // largest character code in font
	auto nw = uint16_t(reader.readUnsigned(2)); // number of words in width table
	auto nh = uint16_t(reader.readUnsigned(2)); // number of words in height table
	auto nd = uint16_t(reader.readUnsigned(2)); // number of words in depth table
	auto ni = uint16_t(reader.readUnsigned(2)); // number of words in italic corr. table
	auto nl = uint16_t(reader.readUnsigned(2)); // number of words in lig/kern table
	auto nk = uint16_t(reader.readUnsigned(2)); // number of words in kern table
	auto ne = uint16_t(reader.readUnsigned(2)); // number of words in ext. char table
	auto np = uint16_t(reader.readUnsigned(2)); // number of font parameter words
	if (6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+nk+ne+np != lf)
		throw FontMetricException("inconsistent length values");
	if (bc >= ec || ec > 255 || ne > 256)
		throw FontMetricException("character codes out of range");
	_firstChar = bc;
	_lastChar = ec;
	readHeader(reader);
	is.seekg(24+lh*4);  // move to char info table
	readTables(reader, nw, nh, nd, ni);
	is.seekg(4*(lf-np));  // move to param section
	readParameters(reader, np);
}


void TFM::readHeader (const StreamReader &reader) {
	_checksum = reader.readUnsigned(4);
	_designSize = double(FixWord(reader.readSigned(4)))*Length::pt2bp;
}


void TFM::readTables (StreamReader &reader, int nw, int nh, int nd, int ni) {
	read_words(reader, _charInfoTable, _lastChar-_firstChar+1);
	read_words(reader, _widthTable, nw);
	read_words(reader, _heightTable, nh);
	read_words(reader, _depthTable, nd);
	read_words(reader, _italicTable, ni);
	_ascent = nh > 0 ? *algo::max_element(_heightTable) : 0;
	_descent = nd > 0 ? *algo::max_element(_depthTable) : 0;
}


/** Read the values from the param section of the TFM file.
 *  @param[in] reader read from this stream
 *  @param[in] np number of parameters to read */
void TFM::readParameters (const StreamReader &reader, int np) {
	_params.resize(7);
	np = min(np, 7);
	for (int i=0; i < np; i++)
		_params[i] = reader.readSigned(4);
	for (int i=np; i < 7; i++)
		_params[i] = 0;
}


/** Returns the optimal space width between words (in PS point units). */
double TFM::getSpace () const {
	return _params.empty() ? 0 : double(_params[1])*_designSize;
}


/** Returns the amount of glue stretching between words (in PS point units). */
double TFM::getSpaceStretch () const {
	return _params.empty() ? 0 : double(_params[2])*_designSize;
}


/** Returns the amount of glue shrinking between words (in PS point units). */
double TFM::getSpaceShrink () const {
	return _params.empty() ? 0 : double(_params[3])*_designSize;
}


/** Returns the size of one EM unit (in PS point units). */
double TFM::getQuad () const {
	if (_params.empty() || _params[5] == 0)
		return _designSize;
	return double(_params[5])*_designSize;
}


/** Returns the index to the entry of the character info table that describes the metric of a given character.
 *  @param[in] c character whose index is retrieved
 *  @return table index for character c, or -1 if there's no entry */
size_t TFM::charIndex (int c) const {
	if (c < _firstChar || c > _lastChar || size_t(c-_firstChar) >= _charInfoTable.size())
		return -1;
	return c-_firstChar;
}


// the char info word for each character consists of 4 bytes holding the following information:
// width index w, height index (h), depth index (d), italic correction index (it),
// tag (tg) and a remainder:
//
// byte 1   | byte 2    | byte 3    | byte 4
// xxxxxxxx | xxxx xxxx | xxxxxx xx | xxxxxxxx
// w        | h    d    | it     tg | remainder

/** Returns the width of char c in PS point units. */
double TFM::getCharWidth (int c) const {
	size_t index = charIndex(c);
	if (index == size_t(-1))
		return 0;
	index = (_charInfoTable[index] >> 24) & 0xFF;
	return double(_widthTable[index]) * _designSize;
}


/** Returns the height of char c in PS point units. */
double TFM::getCharHeight (int c) const {
	size_t index = charIndex(c);
	if (index == size_t(-1))
		return 0;
	index = (_charInfoTable[index] >> 20) & 0x0F;
	return double(_heightTable[index]) * _designSize;
}


/** Returns the depth of char c in PS point units. */
double TFM::getCharDepth (int c) const {
	size_t index = charIndex(c);
	if (index == size_t(-1))
		return 0;
	index = (_charInfoTable[index] >> 16) & 0x0F;
	return double(_depthTable[index]) * _designSize;
}


/** Returns the italic correction of char c in PS point units. */
double TFM::getItalicCorr (int c) const {
	size_t index = charIndex(c);
	if (index == size_t(-1))
		return 0;
	index = (_charInfoTable[index] >> 10) & 0x3F;
	return double(_italicTable[index]) * _designSize;
}
