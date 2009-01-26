/***********************************************************************
** Bitmap.h                                                           **
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

#ifndef BITMAP_H
#define BITMAP_H

#include <ostream>
#include "types.h"

using std::ostream;

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
      ~Bitmap ();
		void resize (int minx, int maxx, int miny , int maxy);
		void setBits(int r, int c, int n);
		const UInt8* operator[] (int r) const {return _bytes+r*_bpr;}
		int height () const                   {return _rows;}
		int width () const                    {return _cols;}
		int xshift () const                   {return _xshift;}
		int yshift () const                   {return _yshift;}
		int bytesPerRow () const              {return _bpr;}
		bool empty () const                   {return (!_rows && !_cols) || !_bytes;}
		void bbox (int &w, int &h) const;
		void forAllPixels (ForAllData &data) const;
		
		template <typename T>
		int copy (T* &target, bool vflip=false) const;
			
		ostream& write (ostream &os) const;

   private:
		int _rows, _cols;     ///< number of rows, columns
		int _xshift, _yshift; ///< horizontal/vertical shift
		int _bpr;             ///< number of bytes per row
		UInt8 *_bytes;
};


/** Copies the bitmap to a new target area and reorganize the bits. 
 *  @param[out] target points to first T of new bitmap (must be deleted after usage)
 *  @param[in]  vflip true if the new bitmap should be flipped vertically 
 *  @return number of Ts per row */
template <typename T>
int Bitmap::copy (T* &target, bool vflip) const {
	const int s = sizeof(T);
	const int tpr = _bpr/s + (_bpr%s ? 1 : 0); // number of Ts per row
	target = new T[_rows*tpr];
	for (int r=0; r < _rows; r++) {
		int targetrow = vflip ? _rows-r-1 : r;
		for (int b=0; b < _bpr; b++) {
			T &t = target[targetrow*tpr + b/s];
			T chunk = _bytes[r*_bpr+b] << (8*(s-1-b%s));
			if (b % s == 0)
				t = chunk;
			else
				t |= chunk;
		}
	}
	return tpr;
}

#endif
