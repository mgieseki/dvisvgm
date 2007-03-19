/***********************************************************************
** TFM.cpp                                                            **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/
// $Id$

#include <iostream>
#include <fstream>
#include <vector>
#include "KPSFileFinder.h"
#include "Message.h"
#include "MetafontWrapper.h"
#include "TFM.h"

using namespace std;

double TFM::mag = 1;


/** Reads a single unsigned integer value of given size (max. 4 bytes). 
 *  @param n number of bytes to be read */
static UInt32 read_unsigned (istream &is, int n) {
	UInt32 ret = 0;
	for (int i=n-1; i >= 0 && !is.eof(); i--) {
		UInt32 b = is.get();
		ret |= b << (8*i);
	}
	return ret;
}


/** Reads a sequence of n TFM words (4 Bytes each). 
 *  @return dynamically allocated array containing the read words 
 *          (must be deleted by the caller) */
template <typename T>
static void read_words (istream &is, vector<T> &v, unsigned n) {
	v.clear();
	v.resize(n);
	for (unsigned i=0; i < n; i++)
		v[i] = read_unsigned(is, 4);
}


/** Converts a TFM fix point value to double. */
static double fix2double (FixWord fix) {
	return double(fix)/(1 << 20);
}


TFM::TFM () : checksum(0), firstChar(0), lastChar(0), designSize(0)
{
}


TFM::TFM (istream &is) {
	readFromStream(is);
}


TFM* TFM::createFromFile (const char *fontname) {
	string filename = string(fontname) + ".tfm";
	const char *path = KPSFileFinder::lookup(filename);
	ifstream ifs(path, ios_base::binary);
	if (ifs)
		return new TFM(ifs);
	return 0;
}


bool TFM::readFromStream (istream &is) {
	is.seekg(2, ios_base::beg);        // skip file size
	UInt16 lh = read_unsigned(is, 2);  // length of header in 4 byte words
	firstChar = read_unsigned(is, 2);  // smalles character code in font
	lastChar  = read_unsigned(is, 2);  // largest character code in font
	UInt16 nw = read_unsigned(is, 2);  // number of words in width table
	UInt16 nh = read_unsigned(is, 2);  // number of words in height table
	UInt16 nd = read_unsigned(is, 2);  // number of words in depth table
//	UInt16 ni = read_unsigned(is, 2);  // number of words in italic corr. table
//	UInt16 nl = read_unsigned(is, 2);  // number of words in lig/kern table
//	UInt16 nk = read_unsigned(is, 2);  // number of words in kern table
//	UInt16 ne = read_unsigned(is, 2);  // number of words in ext. char table
//	UInt16 np = read_unsigned(is, 2);  // number of font parameter words 
	
	is.seekg(10, ios_base::cur);       // move to header
	checksum = read_unsigned(is, 4);
	designSize = read_unsigned(is, 4);
	is.seekg(24+lh*4, ios_base::beg);  // move to char info table
	read_words(is, charInfoTable, lastChar-firstChar+1);
	read_words(is, widthTable, nw);
	read_words(is, heightTable, nh);
	read_words(is, depthTable, nd);
	return true;
}


/** Returns the design size of this font in TeX point units. */
double TFM::getDesignSize () const {
	return fix2double(designSize);
}


/** Returns the width of char c in TeX point units. */
double TFM::getCharWidth (int c) const {
	if (c < firstChar || c > lastChar || unsigned(c-firstChar) >= charInfoTable.size())
		return 0;   
	int index = (charInfoTable[c-firstChar] >> 24) & 0xFF;
	return fix2double(widthTable[index]) * fix2double(designSize);
}


/** Returns the height of char c in TeX point units. */
double TFM::getCharHeight (int c) const {
	if (c < firstChar || c > lastChar || unsigned(c-firstChar) >= charInfoTable.size())
		return 0;   
	int index = (charInfoTable[c-firstChar] >> 20) & 0x0F;
	return fix2double(heightTable[index]) * fix2double(designSize);
}


/** Returns the depth of char c in TeX point units. */
double TFM::getCharDepth (int c) const {
	if (c < firstChar || c > lastChar || unsigned(c-firstChar) >= charInfoTable.size())
		return 0;   
	int index = (charInfoTable[c-firstChar] >> 16) & 0x0F;
	return fix2double(depthTable[index]) * fix2double(designSize);
}

