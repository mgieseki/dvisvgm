/*************************************************************************
** CharMap.cpp                                                          **
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
#include "CharMap.h"

using namespace std;

static bool is_less (const pair<UInt32,UInt32> &p1, const pair<UInt32,UInt32> &p2) {
	return p1.first < p2.first;
}


UInt32 CharMap::operator [] (UInt32 c) const {
	vector<UInt32Pair>::const_iterator it = lower_bound(_pairs.begin(), _pairs.end(), UInt32Pair(c, 0), &is_less);
	return (it != _pairs.end() && it->first == c) ? it->second : 0;
}


/** Returns true if the charmap contains an entry that maps to the given charcode */
bool CharMap::valueExists (UInt32 c) const {
	for (std::vector<UInt32Pair>::const_iterator it=_pairs.begin(); it != _pairs.end(); ++it)
		if (it->second == c)
			return true;
	return false;
}


/** Swaps key and value of all entries. */
void CharMap::invert () {
	for (std::vector<UInt32Pair>::iterator it=_pairs.begin(); it != _pairs.end(); ++it)
		swap(it->first, it->second);
	sort();
}


void CharMap::sort () {
	std::sort(_pairs.begin(), _pairs.end(), &is_less);
}
