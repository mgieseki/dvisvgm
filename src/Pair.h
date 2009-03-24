/***********************************************************************
** Pair.h                                                             **
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

#ifndef PAIR_H
#define PAIR_H

#include <cmath>
#include <ostream>
#include "macros.h"

template <typename T>
class Pair
{
   public:
      Pair (T x=0, T y=0) : _x(x), _y(y) {}
		Pair operator += (const Pair &p)       {_x += p._x; _y += p._y; return *this;}
		Pair operator -= (const Pair &p)       {_x -= p._x; _y -= p._y; return *this;}
		Pair operator *= (T c)                 {_x *= c; _y *= c; return *this;}
		Pair operator /= (T c)                 {_x /= c; _y /= c; return *this;}
      Pair ortho () const                    {return Pair(-_y, _x);}
      double length () const                 {return std::sqrt(_x*_x + _y*_y);}
		bool operator == (const Pair &p) const {return _x == p._x && _y == p._y;}
		bool operator != (const Pair &p) const {return _x != p._x || _y != p._y;}
		T x () const                           {return _x;}
		T y () const                           {return _y;}
		void x (const T &xx)                   {_x = xx;}
		void y (const T &yy)                   {_y = yy;}
		std::ostream& write (std::ostream &os) const {return os << '(' << _x << ',' << _y << ')';}

   private:
		T _x, _y;
};


struct LPair : public Pair<long>
{
   LPair (long x=0, long y=0) : Pair<long>(x, y) {}
	explicit LPair (double x, double y) : Pair<long>(long(x+0.5), long(y+0.5)) {}
	LPair (const Pair<long> &p) : Pair<long>(p) {}
	operator Pair<long> () {return *this;}
};

typedef Pair<double> DPair;

template <typename T>
IMPLEMENT_ARITHMETIC_OPERATOR(Pair<T>, +)
	
template <typename T>
IMPLEMENT_ARITHMETIC_OPERATOR(Pair<T>, -)
	
template <typename T>
IMPLEMENT_ARITHMETIC_OPERATOR2(Pair<T>, T, *)

template <typename T>
IMPLEMENT_ARITHMETIC_OPERATOR2(Pair<T>, T, /)

template <typename T>
IMPLEMENT_OUTPUT_OPERATOR(Pair<T>)

IMPLEMENT_ARITHMETIC_OPERATOR2(LPair, long, *)
#endif
