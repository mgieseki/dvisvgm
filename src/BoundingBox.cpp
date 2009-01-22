/***********************************************************************
** BoundingBox.cpp                                                    **
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

#include <algorithm>
#include <sstream>
#include <string>
#include "BoundingBox.h"
#include "Pair.h"
#include "TransformationMatrix.h"

using namespace std;


BoundingBox::BoundingBox () 
	: ulx(0), uly(0), lrx(0), lry(0), valid(false)
{
}


BoundingBox::BoundingBox (double ulxx, double ulyy, double lrxx, double lryy)
	: ulx(min(ulxx,lrxx)), uly(min(ulyy,lryy)), 
	  lrx(max(ulxx,lrxx)), lry(max(ulyy,lryy)), 
	  valid(true)
{
}


void BoundingBox::embed (double x, double y) {
	if (valid) {
		if (x < ulx) 
			ulx = x;
		else if (x > lrx) 
			lrx = x;
		if (y < uly) 
			uly = y;
		else if (y > lry) 
			lry = y;
	}
	else {
		ulx = lrx = x;
		uly = lry = y;
		valid = true;
	}
}


void BoundingBox::embed (const BoundingBox &bb) {
	if (bb.valid) {
		if (valid) {
			embed(bb.ulx, bb.uly);
			embed(bb.lrx, bb.lry);
		}
		else {
			ulx = bb.ulx;
			uly = bb.uly;
			lrx = bb.lrx;
			lry = bb.lry;
			valid = true;
		}
	}
}


void BoundingBox::operator += (const BoundingBox &bb) {
	ulx += bb.ulx;
	uly += bb.uly;
	lrx += bb.lrx;
	lry += bb.lry;
}


void BoundingBox::transform (const TransformationMatrix &tm) {
	DPair ul = tm * DPair(lrx, lry);
	DPair lr = tm * DPair(ulx, uly);
	DPair ll = tm * DPair(ulx, lry);
	DPair ur = tm * DPair(lrx, uly);
	ulx = min(min(ul.getX(), lr.getX()), min(ur.getX(), ll.getX()));
	uly = min(min(ul.getY(), lr.getY()), min(ur.getY(), ll.getY()));
	lrx = max(max(ul.getX(), lr.getX()), max(ur.getX(), ll.getX()));
	lry = max(max(ul.getY(), lr.getY()), max(ur.getY(), ll.getY()));
}


string BoundingBox::toSVGViewBox () const {
	ostringstream oss;
	oss << ulx << ' ' << uly << ' ' << width() << ' ' << height();
	return oss.str();
}


ostream& BoundingBox::write (ostream &os) const {
	return os << '('  << ulx << ", " << uly
				 << ", " << lrx << ", " << lry << ')';
}
