/***********************************************************************
** Pair.h                                                             **
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

#ifndef PAIR_H
#define PAIR_H

#include <ostream>
#include "macros.h"

template <typename T>
class Pair
{
   public:
      Pair (T xx=0, T yy=0) : x(xx), y(yy) {}
		Pair operator += (const Pair &p)        {x += p.x; y += p.y; return *this;}
		Pair operator -= (const Pair &p)        {x -= p.x; y -= p.y; return *this;}
		Pair operator *= (T c)                  {x *= c;   y *= c;   return *this;}
		Pair operator /= (T c)                  {x /= c;   y /= c;   return *this;}
		bool operator == (const Pair &p) const  {return x == p.x && y == p.y;}
		bool operator != (const Pair &p) const  {return x != p.x || y != p.y;}
		T getX () const                         {return x;}
		T getY () const                         {return y;}
		std::ostream& write (std::ostream &os) const {return os << '(' << x << ',' << y << ')';}

   private:
		T x, y;
};


struct LPair : public Pair<long>
{
   LPair (long xx=0, long yy=0) : Pair<long>(xx, yy) {}
	explicit LPair (double xx, double yy) : Pair<long>(long(xx+0.5), long(yy+0.5)) {}
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

#endif
