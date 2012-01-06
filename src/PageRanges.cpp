/*************************************************************************
** PageRanges.cpp                                                       **
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

#include <sstream>
#include "InputBuffer.h"
#include "InputReader.h"
#include "PageRanges.h"

#include "macros.h"

using namespace std;


/** Analyzes a string describing a range sequence.
 *  Syntax: ([0-9]+(-[0-9]*)?)|(-[0-9]+)(,([0-9]+(-[0-9]*)?)|(-[0-9]+))*
 *  @param[in] str string to parse
 *  @param[in] max_page greatest allowed value
 *  @param[out] ranges the extracted range sequence
 *  @return true on success; false denotes a syntax error */
bool PageRanges::parse (string str, int max_page) {
	StringInputBuffer ib(str);
	BufferInputReader ir(ib);
	while (ir) {
		int first=1;
		int last=max_page;
		ir.skipSpace();
		if (!isdigit(ir.peek()) && ir.peek() != '-')
			return false;

		if (isdigit(ir.peek()))
			first = ir.getInt();
		ir.skipSpace();
		if (ir.peek() == '-') {
			while (ir.peek() == '-')
				ir.get();
			ir.skipSpace();
			if (isdigit(ir.peek()))
				last = ir.getInt();
		}
		else
			last = first;
		ir.skipSpace();
		if (ir.peek() == ',') {
			ir.get();
			if (ir.eof())
				return false;
		}
		else if (!ir.eof())
			return false;
		if (first > last)
			swap(first, last);
		first = max(1, first);
		last  = max(first, last);
		if (max_page > 0) {
			first = min(first, max_page);
			last  = min(last, max_page);
		}
		insert(first, last);
	}
	return true;
}


/** Adds a page range to the range collection.
 *  @param[in] first first page of new range
 *  @param[in] last last page of new range */
void PageRanges::insert (int first, int last) {
	if (first > last)
		swap(first, last);
	Container::iterator it = _ranges.begin();
	while (it != _ranges.end() && first > it->first+1 && first > it->second+1)
		++it;
	if (it == _ranges.end() || last < it->first-1 || first > it->second+1)
		it = _ranges.insert(it, Range(first, last));
	else if ((first < it->first && last >= it->first-1) || (first <= it->second+1 && last > it->second)) {
		it->first = min(it->first, first);
		it->second = max(it->second, last);
	}
	// merge adjacent ranges
	if (it != _ranges.end()) {
		Container::iterator l = it;
		Container::iterator r = it;
		if (l == _ranges.begin())
			l = _ranges.end();
		else
			--l;
		++r;
		bool l_modified = false;
		bool r_modified = false;
		if (l != _ranges.end() && l->second >= it->first-1) {
			l->first = min(l->first, it->first);
			l->second = max(l->second, it->second);
			l_modified = true;
		}
		if (r != _ranges.end() && r->first <= it->second+1) {
			r->first = min(r->first, it->first);
			r->second = max(r->second, it->second);
			r_modified = true;
		}
		if (l_modified || r_modified) {
			_ranges.erase(it);
			if (l_modified && r_modified && l->second >= r->first-1) {
				l->first = min(l->first, r->first);
				l->second = max(l->second, r->second);
				_ranges.erase(r);
			}
		}
	}
}


/** Returns the number of pages. */
size_t PageRanges::pages () const {
	size_t sum=0;
	for (ConstIterator it=_ranges.begin(); it != _ranges.end(); ++it)
		sum += it->second - it->first + 1;
	return sum;
}
