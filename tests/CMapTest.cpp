/*************************************************************************
** CMapTest.cpp                                                         **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <gtest/gtest.h>
#include "CMap.hpp"

using namespace std;

#define CHECK_RANGE(name, cmap, cmin, cmax, cid_min) \
	{SCOPED_TRACE(name); check_range(cmap, cmin, cmax, cid_min);}

#define CHECK_ZERO(name, cmap, cmin, cmax) \
	{SCOPED_TRACE(name); check_zero(cmap, cmin, cmax);}


static void check_range (const SegmentedCMap &cmap, int min, int max, int cid_min) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(cmap.cid(i), uint32_t(cid_min+(i-min)));
}


static void check_zero (const SegmentedCMap &cmap, int min, int max) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(cmap.cid(i), 0u);
}


TEST(SegmentedCMapTest, disjoint_ranges) {
	SegmentedCMap cmap("test");
	cmap.addCIDRange(5, 8, 1);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("A", cmap, 5, 8, 1);
	CHECK_ZERO("B", cmap, 0, 4);
	CHECK_ZERO("C", cmap, 9, 20);

	cmap.addCIDRange(10, 15, 5);
	ASSERT_EQ(cmap.numCIDRanges(), 2u);
	CHECK_RANGE("D", cmap, 5, 8, 1);
	CHECK_RANGE("E", cmap, 10, 15, 5);
	CHECK_ZERO("F", cmap, 0, 4);
	CHECK_ZERO("G", cmap, 9, 9);
	CHECK_ZERO("H", cmap, 16, 20);

	cmap.addCIDRange(0, 3, 50);
	ASSERT_EQ(cmap.numCIDRanges(), 3u);
	CHECK_RANGE("I", cmap, 0, 3, 50);
	CHECK_RANGE("J", cmap, 5, 8, 1);
	CHECK_RANGE("K", cmap, 10, 15, 5);
	CHECK_ZERO("L", cmap, 4, 4);
	CHECK_ZERO("M", cmap, 9, 9);
	CHECK_ZERO("N", cmap, 16, 20);

	cmap.addCIDRange(16, 20, 1);
	ASSERT_EQ(cmap.numCIDRanges(), 4u);
	CHECK_RANGE("O", cmap, 5, 8, 1);
	CHECK_RANGE("P", cmap, 10, 15, 5);
	CHECK_RANGE("Q", cmap, 0, 3, 50);
	CHECK_RANGE("R", cmap, 16, 20, 1);
}


TEST(SegmentedCMapTest, touching_ranges) {
	SegmentedCMap cmap("test");
	cmap.addCIDRange(5, 8, 10);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("A", cmap, 5, 8, 10);

	cmap.addCIDRange(9, 15, 14);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("B", cmap, 5, 15, 10);

	cmap.addCIDRange(1, 4, 5);
	ASSERT_EQ(cmap.numCIDRanges(), 2u);
	CHECK_RANGE("C", cmap, 1, 4, 5);
	CHECK_RANGE("D", cmap, 5, 15, 10);

	cmap.addCIDRange(1, 4, 6);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("E", cmap, 1, 15, 6);
}


TEST(SegmentedCMapTest, overlapping_ranges) {
	SegmentedCMap cmap("test");
	cmap.addCIDRange(5, 8, 10);
	cmap.addCIDRange(7, 15, 12);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("A", cmap, 5, 15, 10);

	cmap.addCIDRange(10, 20, 1);
	ASSERT_EQ(cmap.numCIDRanges(), 2u);
	CHECK_RANGE("B", cmap, 5, 9, 10);
	CHECK_RANGE("C", cmap, 10, 20, 1);

	cmap.addCIDRange(2, 7, 7);
	ASSERT_EQ(cmap.numCIDRanges(), 2u);
	CHECK_RANGE("D", cmap, 2, 9, 7);
	CHECK_RANGE("E", cmap, 10, 20, 1);

	cmap.addCIDRange(1, 12, 100);
	ASSERT_EQ(cmap.numCIDRanges(), 2u);
	CHECK_RANGE("F", cmap, 1, 12, 100);
	CHECK_RANGE("G", cmap, 13, 20, 4);

	cmap.addCIDRange(0, 30, 1);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("H", cmap, 0, 30, 1);
}


TEST(SegmentedCMapTest, inner_ranges) {
	SegmentedCMap cmap("test");
	cmap.addCIDRange(5, 20, 1);
	cmap.addCIDRange(10, 15, 6);
	ASSERT_EQ(cmap.numCIDRanges(), 1u);
	CHECK_RANGE("A", cmap, 5, 20, 1);

	cmap.addCIDRange(10, 15, 100);
	ASSERT_EQ(cmap.numCIDRanges(), 3u);
	CHECK_RANGE("B", cmap, 5, 9, 1);
	CHECK_RANGE("C", cmap, 10, 15, 100);
	CHECK_RANGE("D", cmap, 16, 20, 12);

	cmap.addCIDRange(15, 15, 50);
	ASSERT_EQ(cmap.numCIDRanges(), 4u);
	CHECK_RANGE("E", cmap, 5, 9, 1);
	CHECK_RANGE("F", cmap, 10, 14, 100);
	CHECK_RANGE("G", cmap, 15, 15, 50);
	CHECK_RANGE("H", cmap, 16, 20, 12);

	cmap.addCIDRange(6, 19, 1);
	ASSERT_EQ(cmap.numCIDRanges(), 3u);
	CHECK_RANGE("I", cmap, 5, 5, 1);
	CHECK_RANGE("J", cmap, 6, 19, 1);
	CHECK_RANGE("K", cmap, 20, 20, 16);
}
