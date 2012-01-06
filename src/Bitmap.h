/*************************************************************************
** Bitmap.h                                                             **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef BITMAP_H
#define BITMAP_H

#include <ostream>
#include <vector>
#include "types.h"


class Bitmap
{
	public:
		struct ForAllData {
			virtual ~ForAllData() {}
			virtual void pixel (int x, int y, bool set, Bitmap &bm) {}
			virtual void pixel (int x, int y, bool set, const Bitmap &bm) {}
		};

   public:
      Bitmap ();
		Bitmap (int minx, int maxx, int miny , int maxy);
		void resize (int minx, int maxx, int miny , int maxy);
		void setBits(int r, int c, int n);
		const UInt8* operator[] (int r) const {return &_bytes[r*_bpr];}
		int height () const                   {return _rows;}
		int width () const                    {return _cols;}
		int xshift () const                   {return _xshift;}
		int yshift () const                   {return _yshift;}
		int bytesPerRow () const              {return _bpr;}
		bool empty () const                   {return (!_rows && !_cols) || _bytes.empty();}
		void bbox (int &w, int &h) const;
		void forAllPixels (ForAllData &data) const;

		template <typename T>
		int copy (std::vector<T> &target, bool vflip=false) const;

//		template <typename T>
//		void write (std::ostream &os, const std::vector<T> &v) const;

		std::ostream& write (std::ostream &os) const;

   private:
		int _rows, _cols;     ///< number of rows, columns
		int _xshift, _yshift; ///< horizontal/vertical shift
		int _bpr;             ///< number of bytes per row
		std::vector<UInt8> _bytes;
};


/** Copies the bitmap to a new target area and reorganize the bits.
 *  @param[out] target points to first T of new bitmap (must be deleted after usage)
 *  @param[in]  vflip true if the new bitmap should be flipped vertically
 *  @return number of Ts per row */
template <typename T>
int Bitmap::copy (std::vector<T> &target, bool vflip) const {
	const int s = sizeof(T);
	const int tpr = _bpr/s + (_bpr%s ? 1 : 0); // number of Ts per row
	target.resize(_rows*tpr);
	for (int r=0; r < _rows; r++) {
		int targetrow = vflip ? _rows-r-1 : r;
		for (int b=0; b < _bpr; b++) {
			T &t = target[targetrow*tpr + b/s];
			T chunk = (T)_bytes[r*_bpr+b] << (8*(s-1-b%s));
			if (b % s == 0)
				t = chunk;
			else
				t |= chunk;
		}
	}
	return tpr;
}


/*
template <typename T>
void Bitmap::write (std::ostream &os, const std::vector<T> &v) const {
	const int s = sizeof(T);
	const int tpr = _bpr/s + (_bpr%s ? 1 : 0); // number of Ts per row
	for (int r=_rows-1; r >= 0; r--) {
		for (int t=0; t < tpr; t++) {
			for (T b=(T)1<<(8*s-1); b; b>>=1)
				os << ((v[r*tpr+t] & b) ? '*' : '-');
			os << ' ';
		}
		os << std::endl;
	}
}*/


#endif
