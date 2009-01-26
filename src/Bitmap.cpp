/***********************************************************************
** Bitmap.cpp                                                         **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#include <cstdlib>
#include <iostream>
#include "Bitmap.h"

using namespace std;

Bitmap::Bitmap () 
	: _rows(0), _cols(0), _xshift(0), _yshift(0), _bpr(0), _bytes(0)
{
}


/** Constructs a Bitmap */
Bitmap::Bitmap (int minx, int maxx, int miny , int maxy) 
	: _bytes(0)
{
	resize(minx, maxx, miny, maxy);
}


Bitmap::~Bitmap () {
	delete [] _bytes;
}


/** Resizes the bitmap and clears all pixels.
 *  @param[in] minx index of leftmost pixel column
 *  @param[in] maxx index of rightmost pixel column
 *  @param[in] miny index of bottom row
 *  @param[in] maxy index of top row */
void Bitmap::resize (int minx, int maxx, int miny , int maxy) {
	_rows = abs(maxy-miny)+1;
	_cols = abs(maxx-minx)+1;
	_xshift = minx;
	_yshift = miny;
	_bpr  = _cols/8 + (_cols % 8 ? 1 : 0);  // bytes per row
	delete [] _bytes;
	_bytes = new UInt8[_rows*_bpr];
	for (UInt8 *p=_bytes+_rows*_bpr-1; p >= _bytes; p--)
		*p = 0;
}


/** Sets n pixels of row r to 1 starting at pixel c. 
 *  @param[in] r number of row
 *  @param[in] c number of column (pixel)
 *  @param[in] n number of bits to be set */
void Bitmap::setBits (int r, int c, int n) {
	r -= _yshift;
	c -= _xshift;
	UInt8 *byte = _bytes + r*_bpr + c/8;// + (c%8 ? 1 : 0);
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


void Bitmap::forAllPixels (ForAllData &data) const {
	for (int r=_rows-1; r >= 0 ; r--) {
		for (int c=0; c < _bpr; c++) {
			UInt8 byte = _bytes[r*_bpr+c];
			for (int b=7; b >= 0; b--) {
				data.pixel(8*c+(7-b), r, byte & (1 << b), *this);
			}
		}
	}
}


struct BBoxData : public Bitmap::ForAllData
{
	BBoxData () : maxx(0), maxy(0) {}
	void pixel (int x, int y, bool set, const Bitmap &bm) {
		if (set) {
			maxx = max(maxx, x);
			maxy = max(maxy, y);
		}
	}
	int maxx, maxy;
};


void Bitmap::bbox (int &w, int &h) const {
	BBoxData data;
	forAllPixels(data);
	w = data.maxx+1;
	h = data.maxy+1;
}


ostream& Bitmap::write (ostream &os) const {
#if 0
	for (int r=_rows-1; r >= 0 ; r--) {
		for (int c=0; c < _bpr; c++) {
			UInt8 byte = _bytes[r*_bpr+c];
			for (int b=128; b; b>>=1) 
				os << (byte & b ? '*' : '-');
			os << ' ';
		}
		os << endl;
	}
#endif
	return os;	
}
