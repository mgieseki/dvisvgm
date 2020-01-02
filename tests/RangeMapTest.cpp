/*************************************************************************
** RangeMapTest.cpp                                                     **
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
#include "RangeMap.hpp"

using namespace std;

#define CHECK_RANGE(name, cmap, cmin, cmax, cid_min) \
	{SCOPED_TRACE(name); check_range(cmap, cmin, cmax, cid_min);}

#define CHECK_ZERO(name, cmap, cmin, cmax) \
	{SCOPED_TRACE(name); check_zero(cmap, cmin, cmax);}


static void check_range (const RangeMap &rangemap, int min, int max, int minval) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ((int)rangemap.valueAt(i), minval+(i-min));
}


static void check_zero (const RangeMap &rangemap, int min, int max) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(rangemap.valueAt(i), 0u);
}


TEST(RangeMapTest, disjoint_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 1);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	EXPECT_EQ(rangemap.numValues(), 4u);
	CHECK_RANGE("A", rangemap, 5, 8, 1);
	CHECK_ZERO("B", rangemap, 0, 4);
	CHECK_ZERO("C", rangemap, 9, 20);

	rangemap.addRange(10, 15, 5);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	EXPECT_EQ(rangemap.numValues(), 10u);
	CHECK_RANGE("D", rangemap, 5, 8, 1);
	CHECK_RANGE("E", rangemap, 10, 15, 5);
	CHECK_ZERO("F", rangemap, 0, 4);
	CHECK_ZERO("G", rangemap, 9, 9);
	CHECK_ZERO("H", rangemap, 16, 20);

	rangemap.addRange(0, 3, 50);
	ASSERT_EQ(rangemap.numRanges(), 3u);
	EXPECT_EQ(rangemap.numValues(), 14u);
	CHECK_RANGE("I", rangemap, 0, 3, 50);
	CHECK_RANGE("J", rangemap, 5, 8, 1);
	CHECK_RANGE("K", rangemap, 10, 15, 5);
	CHECK_ZERO("L", rangemap, 4, 4);
	CHECK_ZERO("M", rangemap, 9, 9);
	CHECK_ZERO("N", rangemap, 16, 20);

	rangemap.addRange(16, 20, 1);
	ASSERT_EQ(rangemap.numRanges(), 4u);
	EXPECT_EQ(rangemap.numValues(), 19u);
	CHECK_RANGE("O", rangemap, 5, 8, 1);
	CHECK_RANGE("P", rangemap, 10, 15, 5);
	CHECK_RANGE("Q", rangemap, 0, 3, 50);
	CHECK_RANGE("R", rangemap, 16, 20, 1);
}


TEST(RangeMapTest, touching_ranges1) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 10);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	EXPECT_EQ(rangemap.numValues(), 4u);
	CHECK_RANGE("A", rangemap, 5, 8, 10);

	rangemap.addRange(9, 15, 14);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	EXPECT_EQ(rangemap.numValues(), 11u);
	CHECK_RANGE("B", rangemap, 5, 15, 10);

	rangemap.addRange(1, 4, 5);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	EXPECT_EQ(rangemap.numValues(), 15u);
	CHECK_RANGE("C", rangemap, 1, 4, 5);
	CHECK_RANGE("D", rangemap, 5, 15, 10);

	rangemap.addRange(1, 4, 6);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	EXPECT_EQ(rangemap.numValues(), 15u);
	CHECK_RANGE("E", rangemap, 1, 15, 6);
}


TEST(RangeMapTest, touching_ranges2) {
	RangeMap rangemap;
	rangemap.addRange(7, 8, 10);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	CHECK_RANGE("A", rangemap, 7, 8, 10);

	rangemap.addRange(2, 3, 1);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	CHECK_RANGE("B", rangemap, 2, 3, 1);
	CHECK_RANGE("C", rangemap, 7, 8, 10);

	rangemap.addRange(4, 4, 3);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	CHECK_RANGE("D", rangemap, 2, 4, 1);
	CHECK_RANGE("E", rangemap, 7, 8, 10);
}


TEST(RangeMapTest, overlapping_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 10);
	rangemap.addRange(7, 15, 12);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	CHECK_RANGE("A", rangemap, 5, 15, 10);

	rangemap.addRange(10, 20, 1);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	CHECK_RANGE("B", rangemap, 5, 9, 10);
	CHECK_RANGE("C", rangemap, 10, 20, 1);

	rangemap.addRange(2, 7, 7);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	CHECK_RANGE("D", rangemap, 2, 9, 7);
	CHECK_RANGE("E", rangemap, 10, 20, 1);

	rangemap.addRange(1, 12, 100);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	CHECK_RANGE("F", rangemap, 1, 12, 100);
	CHECK_RANGE("G", rangemap, 13, 20, 4);

	rangemap.addRange(0, 30, 1);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	CHECK_RANGE("H", rangemap, 0, 30, 1);
}


TEST(RangeMapTest, inner_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 20, 1);
	rangemap.addRange(10, 15, 6);
	ASSERT_EQ(rangemap.numRanges(), 1u);
	CHECK_RANGE("A", rangemap, 5, 20, 1);

	rangemap.addRange(10, 15, 100);
	ASSERT_EQ(rangemap.numRanges(), 3u);
	CHECK_RANGE("B", rangemap, 5, 9, 1);
	CHECK_RANGE("C", rangemap, 10, 15, 100);
	CHECK_RANGE("D", rangemap, 16, 20, 12);

	rangemap.addRange(15, 15, 50);
	ASSERT_EQ(rangemap.numRanges(), 4u);
	CHECK_RANGE("E", rangemap, 5, 9, 1);
	CHECK_RANGE("F", rangemap, 10, 14, 100);
	CHECK_RANGE("G", rangemap, 15, 15, 50);
	CHECK_RANGE("H", rangemap, 16, 20, 12);

	rangemap.addRange(6, 19, 1);
	ASSERT_EQ(rangemap.numRanges(), 3u);
	CHECK_RANGE("I", rangemap, 5, 5, 1);
	CHECK_RANGE("J", rangemap, 6, 19, 1);
	CHECK_RANGE("K", rangemap, 20, 20, 16);
}


TEST(RangeMapTest, iterators) {
	RangeMap rangemap;
	rangemap.addRange(5, 10, 1);
	rangemap.addRange(20, 25, 60);
	ASSERT_EQ(rangemap.numRanges(), 2u);
	using Pair32 = pair<uint32_t,uint32_t>;
	vector<Pair32> pairs = {
		Pair32(5, 1), Pair32(6, 2), Pair32(7, 3), Pair32(8, 4), Pair32(9, 5), Pair32(10, 6),
		Pair32(20, 60), Pair32(21, 61), Pair32(22, 62), Pair32(23, 63), Pair32(24, 64), Pair32(25, 65)
	};
	int count=0;
	for (auto p : rangemap)
		ASSERT_EQ(p, pairs[count++]);
}
