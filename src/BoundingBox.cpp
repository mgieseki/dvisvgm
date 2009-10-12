/*************************************************************************
** BoundingBox.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <algorithm>
#include <sstream>
#include <string>
#include "BoundingBox.h"
#include "Matrix.h"

using namespace std;


BoundingBox::BoundingBox ()
	: ulx(0), uly(0), lrx(0), lry(0), _valid(false), _locked(false)
{
}


BoundingBox::BoundingBox (double ulxx, double ulyy, double lrxx, double lryy)
	: ulx(min(ulxx,lrxx)), uly(min(ulyy,lryy)),
	  lrx(max(ulxx,lrxx)), lry(max(ulyy,lryy)),
	  _valid(true), _locked(false)
{
}


BoundingBox::BoundingBox (const DPair &p1, const DPair &p2)
	: ulx(min(p1.x(), p2.x())), uly(min(p1.y(), p2.y())),
	  lrx(max(p1.x(), p2.x())), lry(max(p1.y(), p2.y())),
	  _valid(true), _locked(false)
{
}


BoundingBox::BoundingBox (const Length &ulxx, const Length &ulyy, const Length &lrxx, const Length &lryy)
	: ulx(min(ulxx.pt(),lrxx.pt())), uly(min(ulyy.pt(),lryy.pt())),
	  lrx(max(ulxx.pt(),lrxx.pt())), lry(max(ulyy.pt(),lryy.pt())),
	  _valid(true), _locked(false)
{
}


void BoundingBox::set (const string &boxstr) {
	Length coord[4];
	const size_t len = boxstr.length();
	size_t l=0;
	for (int i=0; i < 4; i++) {
		while (l < len && isspace(boxstr[l]))
			l++;
		size_t r=l;
		while (r < len && !isspace(boxstr[r]) && boxstr[r] != ',')
			r++;
		string lenstr = boxstr.substr(l, r-l);
		coord[i].set(lenstr);
		if (boxstr[r] == ',')
			r++;
		l = r;
		if ((l == len && i < 3) || lenstr.empty())
			throw BoundingBoxException("four length parameters expected");
	}
	ulx = min(coord[0].pt(), coord[2].pt());
	uly = min(coord[1].pt(), coord[3].pt());
	lrx = max(coord[0].pt(), coord[2].pt());
	lry = max(coord[1].pt(), coord[3].pt());
}


/** Enlarges the box so that point (x,y) is enclosed. */
void BoundingBox::embed (double x, double y) {
	if (!_locked) {
		if (_valid) {
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
			_valid = true;
		}
	}
}


/** Enlarges the box so that box bb is enclosed. */
void BoundingBox::embed (const BoundingBox &bb) {
	if (!_locked && bb._valid) {
		if (_valid) {
			embed(bb.ulx, bb.uly);
			embed(bb.lrx, bb.lry);
		}
		else {
			ulx = bb.ulx;
			uly = bb.uly;
			lrx = bb.lrx;
			lry = bb.lry;
			_valid = true;
		}
	}
}


void BoundingBox::embed (const DPair &c, double r) {
	embed(BoundingBox(c.x()-r, c.y()-r, c.x()+r, c.y()+r));
}


void BoundingBox::expand (double m) {
	if (!_locked) {
		ulx -= m;
		uly -= m;
		lrx += m;
		lry += m;
	}
}


/** Intersects the current box with bbox and applies the result to *this.
 *  If both boxes are disjoint, *this is not altered.
 *  @param[in] bbox box to intersect with
 *  @return false if *this is locked or both boxes are disjoint */
bool BoundingBox::intersect (const BoundingBox &bbox) {
	if (_locked || lrx < bbox.ulx || lry < bbox.uly || ulx > bbox.lrx || uly > bbox.lry)
		return false;
	ulx = max(ulx, bbox.ulx);
	uly = max(uly, bbox.uly);
	lrx = min(lrx, bbox.lrx);
	lry = min(lry, bbox.lry);
	return true;
}


void BoundingBox::operator += (const BoundingBox &bb) {
	if (!_locked) {
		ulx += bb.ulx;
		uly += bb.uly;
		lrx += bb.lrx;
		lry += bb.lry;
	}
}


void BoundingBox::transform (const Matrix &tm) {
	if (!_locked) {
		DPair ul = tm * DPair(lrx, lry);
		DPair lr = tm * DPair(ulx, uly);
		DPair ll = tm * DPair(ulx, lry);
		DPair ur = tm * DPair(lrx, uly);
		ulx = min(min(ul.x(), lr.x()), min(ur.x(), ll.x()));
		uly = min(min(ul.y(), lr.y()), min(ur.y(), ll.y()));
		lrx = max(max(ul.x(), lr.x()), max(ur.x(), ll.x()));
		lry = max(max(ul.y(), lr.y()), max(ur.y(), ll.y()));
	}
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
