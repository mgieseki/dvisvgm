/*************************************************************************
** CMap.cpp                                                             **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include <algorithm>
#include <sstream>
#include <set>
#include <vector>
#include "CMap.h"
#include "CMapManager.h"
#include "FileFinder.h"
#include "InputReader.h"

using namespace std;


const char* CMap::path () const {
	return FileFinder::lookup(name(), "cmap", false);
}


const FontEncoding* CMap::findCompatibleBaseFontMap (const PhysicalFont *font, CharMapID &charmapID) const {
	return CMapManager::instance().findCompatibleBaseFontMap(font, this, charmapID);
}

//////////////////////////////////////////////////////////////////////

/** Returns the RO (Registry-Ordering) string of the CMap. */
string SegmentedCMap::getROString() const {
	if (_registry.empty() || _ordering.empty())
		return "";
	return _registry + "-" + _ordering;
}


/** Tries to merge range r into this one. This is only possible if the ranges
 *  touch or overlap and if the assigned CIDs match at the junction points.
 *  @param[in] r range to join
 *  @return true if join was successful */
bool SegmentedCMap::Range::join (const Range &r) {
	// check most common cases first
	if (_max+1 < r._min || _min-1 > r._max)  // disjoint ranges?
		return false;
	if (r._min-1 == _max) {                  // does r touch *this on the right?
		if (decode(r._min) == r._cid) {
			_max = r._max;
			return true;
		}
		return false;
	}
	if (r._max+1 == _min) {                  // does r touch *this on the left
		if (r.decode(_min) == _cid) {
			setMinAndAdaptCID(r._min);
			return true;
		}
		return false;
	}
	// the following cases should be pretty rare
	if (r._min <= _min && r._max >= _max) {  // does r overlap *this on both sides?
		*this = r;
		return true;
	}
	if (r._min < _min) {                     // left overlap only?
		if (r.decode(_min) == _cid) {
			_min = r._min;
			_cid = r._cid;
			return true;
		}
		return false;
	}
	if (r._max > _max) {                     // right overlap only?
		if (decode(r._min) == r._cid) {
			_max = r._max;
			return true;
		}
		return false;
	}
	// r completely inside *this
	return decode(r._min) == r._cid;
}


/** Adapts the left and right neighbor elements of a newly inserted range.
 *  The new range could overlap ranges in the neighborhood so that those must be
 *  adapted or removed. All ranges in the range vector are ordered ascendingly, i.e.
 *  [min_1, max_1],...,[min_n, max_n] where min_i < min_j for all i < j.
  * @param[in,out] ranges range collection to be adapted
 *  @param[in] it pointer to the newly inserted range */
void SegmentedCMap::adaptNeighbors (Ranges &ranges, Ranges::iterator it) {
	if (it != ranges.end()) {
		// adapt left neighbor
		SegmentedCMap::Ranges::iterator lit = it-1;    // points to left neighbor
		if (it != ranges.begin() && it->min() <= lit->max()) {
			bool left_neighbor_valid = (it->min() > 0 && it->min()-1 >= lit->min());
			if (left_neighbor_valid)     // is adapted left neighbor valid?
				lit->_max = it->min()-1;  // => assign new max value
			if (!left_neighbor_valid || it->join(*lit))
				it = ranges.erase(lit);
		}
		// remove right neighbors completely overlapped by *it
		SegmentedCMap::Ranges::iterator rit = it+1;    // points to right neighbor
		while (rit != ranges.end() && it->max() >= rit->max()) { // complete overlap?
			ranges.erase(rit);
			rit = it+1;
		}
		// adapt rightmost range partially overlapped by *it
		if (rit != ranges.end()) {
			if (it->max() >= rit->min())
				rit->setMinAndAdaptCID(it->max()+1);
			// try to merge right neighbor into *this
			if (it->join(*rit))
				ranges.erase(rit); // remove merged neighbor
		}
	}
}


/** Adds a new code range. The range describes a mapping from character codes c to CIDs, where
 *  c \in [cmin,cmax] and CID(c):=cid+c-cmin.
 *  @param[in,out] ranges collection the new range is added to
 *  @param[in] cmin smallest character code in the range
 *  @param[in] cmax largest character code in the range
 *  @param[in] cid CID of the smallest character code (cmin) */
void SegmentedCMap::addRange (Ranges &ranges, UInt32 cmin, UInt32 cmax, UInt32 cid) {
	if (cmin > cmax)
		swap(cmin, cmax);

	Range range(cmin, cmax, cid);
	if (ranges.empty())
		ranges.push_back(range);
	else {
		// check for simple cases that can be handled pretty fast
		Range &lrange = *ranges.begin();
		Range &rrange = *ranges.rbegin();
		if (cmin > rrange.max()) {       // non-overlapping range at end of vector?
			if (!rrange.join(range))
				ranges.push_back(range);
		}
		else if (cmax < lrange.min()) {  // non-overlapping range at begin of vector?
			if (!lrange.join(range))
				ranges.insert(ranges.begin(), range);
		}
		else {
			// ranges overlap and/or must be inserted somewhere inside the vector
			Ranges::iterator it = lower_bound(ranges.begin(), ranges.end(), range);
			const bool at_end = (it == ranges.end());
			if (at_end)
				--it;
			if (!it->join(range)) {
				if (it->min() < cmin && it->max() > cmax) { // new range completely inside an existing range?
					//split existing range
					UInt32 itmax = it->max();
					it->_max = cmin-1;
					it = ranges.insert(it+1, Range(cmax+1, itmax, it->decode(cmax+1)));
				}
				else if (at_end)        // does new range overlap right side of last range in vector?
					it = ranges.end();  // => append new range at end of vector
				it = ranges.insert(it, range);
			}
			adaptNeighbors(ranges, it);  // resolve overlaps
		}
	}
}


/** Returns the CID for a given character code. */
UInt32 SegmentedCMap::cid (UInt32 c) const {
	int pos = lookup(_cidranges, c);
	if (pos >= 0)
		return _cidranges[pos].decode(c);
	if (_basemap)
		return _basemap->cid(c);
	return 0;
}


/** Returns the character code of a base font for a given CID. */
UInt32 SegmentedCMap::bfcode (UInt32 cid) const {
	int pos = lookup(_bfranges, cid);
	if (pos >= 0)
		return _bfranges[pos].decode(cid);
	if (_basemap)
		return _basemap->bfcode(cid);
	return 0;
}


/** Finds the index of the range that contains a given value c.
 *  @param[in] c find range that contains this value
 *  @return index of the range found, or -1 if range was not found */
int SegmentedCMap::lookup (const Ranges &ranges, UInt32 c) const {
	// simple binary search
	int left=0, right=ranges.size()-1;
	while (left <= right) {
		int mid = (left+right)/2;
		if (c < ranges[mid].min())
			right = mid-1;
		else if (c > ranges[mid].max())
			left = mid+1;
		else
			return mid;
	}
	return -1;
}


void SegmentedCMap::write (ostream &os) const {
	for (size_t i=0; i < _cidranges.size(); i++) {
		const Range &r = _cidranges[i];
		os << '[' << r.min() << ',' << r.max() << "] => " << r.cid() << '\n';
	}
}
