/***********************************************************************
** Bitmap.cpp                                                         **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id: Bitmap.cpp,v 1.6 2006/01/05 16:05:05 mgieseki Exp $

#include <iostream>
#include "Bitmap.h"

using namespace std;

Bitmap::Bitmap () 
	: rows(0), cols(0), xShift(0), yShift(0), bpr(0), bytes(0)
{
}


Bitmap::Bitmap (int minx, int maxx, int miny , int maxy) 
	: bytes(0)
{
	resize(minx, maxx, miny, maxy);
}


Bitmap::~Bitmap () {
	delete [] bytes;
}


void Bitmap::resize (int minx, int maxx, int miny , int maxy) {
	rows = maxy-miny+1;
	cols = maxx-minx+1;
	xShift = minx;
	yShift = miny;
	bpr  = cols/8 + (cols % 8 ? 1 : 0);
	delete [] bytes;
	bytes = new UInt8[rows*bpr];
	for (UInt8 *p=bytes+rows*bpr-1; p >= bytes; p--)
		*p = 0;
}


/** Sets n pixels of row r starting at pixel c. */
void Bitmap::setBits (int r, int c, int n) {
	r -= yShift;
	c -= xShift;
	UInt8 *byte = bytes + r*bpr + c/8;// + (c%8 ? 1 : 0);
	while (n > 0) {
		int b = 7 - c%8;            // number of leftmost bit in current byte to be set
		int m = min(n, b+1);        // number of bits to be set in current byte
		Int16 bitseq = (1 << m)-1;  // sequence of n set bits (bits 0..n-1 are set)
		bitseq <<= b-m+1;           // move bit sequence so that bit b is the leftmost set bit
		*byte |= bitseq;		       // apply bit sequence to current byte
		byte++;
		n -= m;
		c += m;
	}
}


ostream& Bitmap::write (ostream &os) const {
	for (int r=rows-1; r >= 0 ; r--) {
		for (int c=0; c < bpr; c++) {
			UInt8 byte = bytes[r*bpr+c];
			for (int b=128; b; b>>=1) 
				os << (byte & b ? '*' : '-');
			os << ' ';
		}
		os << endl;
	}
	return os;
}
