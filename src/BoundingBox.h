/***********************************************************************
** BoundingBox.h                                                      **
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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <ostream>
#include <string>
#include "Pair.h"
#include "macros.h"
#include "types.h"


class TransformationMatrix;

class BoundingBox
{
   public:
      BoundingBox ();
		BoundingBox (double ulxx, double ulyy, double lrxx, double lryy);
		BoundingBox (const DPair &p1, const DPair &p2);
		void embed (double x, double y);
		void embed (const BoundingBox &bb);
		void embed (const DPair &p) {embed(p.x(), p.y());}
		void embed (const DPair &c, double r);
		double minX () const        {return ulx;}
		double minY () const        {return uly;}
		double maxX () const        {return lrx;}
		double maxY () const        {return lry;}
		double width () const       {return lrx-ulx;}
		double height () const      {return lry-uly;}
		void operator += (const BoundingBox &bb);
		void transform (const TransformationMatrix &tm);
		std::string toSVGViewBox () const;
		std::ostream& write (std::ostream &os) const;

   private:
		double ulx, uly; //< coordinates of upper left vertex (in TeX point units)
	   double lrx, lry; //< coordinates of lower right vertex (in TeX point unitx)
		bool valid;
};

IMPLEMENT_OUTPUT_OPERATOR(BoundingBox)

#endif
