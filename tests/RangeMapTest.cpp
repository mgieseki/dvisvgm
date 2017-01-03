/*************************************************************************
** RangeMapTest.cpp                                                     **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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


static void check_range (const RangeMap &rangemap, int min, int max, int minval) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(rangemap.valueAt(i), minval+(i-min));
}


static void check_zero (const RangeMap &rangemap, int min, int max) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(rangemap.valueAt(i), 0);
}


TEST(RangeMapTest, disjoint_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 1);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 5, 8, 1);
	check_zero(rangemap, 0, 4);
	check_zero(rangemap, 9, 20);

	rangemap.addRange(10, 15, 5);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 5, 8, 1);
	check_range(rangemap, 10, 15, 5);
	check_zero(rangemap, 0, 4);
	check_zero(rangemap, 9, 9);
	check_zero(rangemap, 16, 20);

	rangemap.addRange(0, 3, 50);
	ASSERT_EQ(rangemap.size(), 3);
	check_range(rangemap, 0, 3, 50);
	check_range(rangemap, 5, 8, 1);
	check_range(rangemap, 10, 15, 5);
	check_zero(rangemap, 4, 4);
	check_zero(rangemap, 9, 9);
	check_zero(rangemap, 16, 20);

	rangemap.addRange(16, 20, 1);
	ASSERT_EQ(rangemap.size(), 4);
	check_range(rangemap, 5, 8, 1);
	check_range(rangemap, 10, 15, 5);
	check_range(rangemap, 0, 3, 50);
	check_range(rangemap, 16, 20, 1);
}


TEST(RangeMapTest, touching_ranges1) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 10);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 5, 8, 10);

	rangemap.addRange(9, 15, 14);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 5, 15, 10);

	rangemap.addRange(1, 4, 5);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 1, 4, 5);
	check_range(rangemap, 5, 15, 10);

	rangemap.addRange(1, 4, 6);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 1, 15, 6);
}


TEST(RangeMapTest, touching_ranges2) {
	RangeMap rangemap;
	rangemap.addRange(7, 8, 10);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 7, 8, 10);

	rangemap.addRange(2, 3, 1);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 2, 3, 1);
	check_range(rangemap, 7, 8, 10);

	rangemap.addRange(4, 4, 3);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 2, 4, 1);
	check_range(rangemap, 7, 8, 10);
}


TEST(RangeMapTest, overlapping_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 8, 10);
	rangemap.addRange(7, 15, 12);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 5, 15, 10);

	rangemap.addRange(10, 20, 1);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 5, 9, 10);
	check_range(rangemap, 10, 20, 1);

	rangemap.addRange(2, 7, 7);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 2, 9, 7);
	check_range(rangemap, 10, 20, 1);

	rangemap.addRange(1, 12, 100);
	ASSERT_EQ(rangemap.size(), 2);
	check_range(rangemap, 1, 12, 100);
	check_range(rangemap, 13, 20, 4);

	rangemap.addRange(0, 30, 1);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 0, 30, 1);
}


TEST(RangeMapTest, inner_ranges) {
	RangeMap rangemap;
	rangemap.addRange(5, 20, 1);
	rangemap.addRange(10, 15, 6);
	ASSERT_EQ(rangemap.size(), 1);
	check_range(rangemap, 5, 20, 1);

	rangemap.addRange(10, 15, 100);
	ASSERT_EQ(rangemap.size(), 3);
	check_range(rangemap, 5, 9, 1);
	check_range(rangemap, 10, 15, 100);
	check_range(rangemap, 16, 20, 12);

	rangemap.addRange(15, 15, 50);
	ASSERT_EQ(rangemap.size(), 4);
	check_range(rangemap, 5, 9, 1);
	check_range(rangemap, 10, 14, 100);
	check_range(rangemap, 15, 15, 50);
	check_range(rangemap, 16, 20, 12);

	rangemap.addRange(6, 19, 1);
	ASSERT_EQ(rangemap.size(), 3);
	check_range(rangemap, 5, 5, 1);
	check_range(rangemap, 6, 19, 1);
	check_range(rangemap, 20, 20, 16);
}
