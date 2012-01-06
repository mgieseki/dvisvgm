/*************************************************************************
** BoundingBox.h                                                        **
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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <ostream>
#include <string>
#include "Length.h"
#include "MessageException.h"
#include "Pair.h"
#include "macros.h"
#include "types.h"


class Matrix;
class XMLElementNode;


struct BoundingBoxException : MessageException
{
	BoundingBoxException (const std::string &msg) : MessageException(msg) {}
};


class BoundingBox
{
   public:
      BoundingBox ();
		BoundingBox (double ulxx, double ulyy, double lrxx, double lryy);
		BoundingBox (const DPair &p1, const DPair &p2);
		BoundingBox (const Length &ulxx, const Length &ulyy, const Length &lrxx, const Length &lryy);
		BoundingBox (const std::string &boxstr) {set(boxstr);}
		void set (std::string boxstr);
		void embed (double x, double y);
		void embed (const BoundingBox &bb);
		void embed (const DPair &p) {embed(p.x(), p.y());}
		void embed (const DPair &c, double r);

		template <typename T>
      void embed (const Pair<T> &p) {embed(p.x(), p.y());}

		void expand (double m);
		bool  intersect (const BoundingBox &bbox);
		double minX () const        {return ulx;}
		double minY () const        {return uly;}
		double maxX () const        {return lrx;}
		double maxY () const        {return lry;}
		double width () const       {return lrx-ulx;}
		double height () const      {return lry-uly;}
		void lock ()                {_locked = true;}
		void unlock ()              {_locked = false;}
		void operator += (const BoundingBox &bb);
      void scale (double sx, double sy);
		void transform (const Matrix &tm);
		std::string toSVGViewBox () const;
		std::ostream& write (std::ostream &os) const;
		XMLElementNode* toSVGRect () const;

   private:
		double ulx, uly; ///< coordinates of upper left vertex (in TeX point units)
	   double lrx, lry; ///< coordinates of lower right vertex (in TeX point unitx)
		bool _valid : 1;  ///< true if the box coordinates are properly set
		bool _locked : 1; ///< if true, the box data is read-only
};

IMPLEMENT_OUTPUT_OPERATOR(BoundingBox)

#endif
