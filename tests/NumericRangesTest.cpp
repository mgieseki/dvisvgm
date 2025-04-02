/*************************************************************************
** NumericRagesTest.cpp                                                 **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "NumericRanges.hpp"

using namespace std;

TEST(NumericRangesTest, construct) {
	NumericRanges<int> ranges{{5,9}, {20,31}};
	EXPECT_EQ(ranges.size(), 2u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 5);
	EXPECT_EQ(it->second, 9);
	++it;
	EXPECT_EQ(it->first, 20);
	EXPECT_EQ(it->second, 31);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange1) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5);
	ranges.addRange (2);
	EXPECT_EQ(ranges.size(), 2u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 2);
	EXPECT_EQ(it->second, 2);
	++it;
	EXPECT_EQ(it->first, 5);
	EXPECT_EQ(it->second, 5);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange2) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5);
	ranges.addRange (4);
	EXPECT_EQ(ranges.size(), 1u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 4);
	EXPECT_EQ(it->second, 5);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange3) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5);
	ranges.addRange (6);
	EXPECT_EQ(ranges.size(), 1u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 5);
	EXPECT_EQ(it->second, 6);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange4) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5, 10);
	ranges.addRange (4, 1);
	EXPECT_EQ(ranges.size(), 1u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 1);
	EXPECT_EQ(it->second, 10);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange5) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5, 10);
	ranges.addRange (1, 8);
	EXPECT_EQ(ranges.size(), 1u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 1);
	EXPECT_EQ(it->second, 10);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange6) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5, 10);
	ranges.addRange (1, 11);
	EXPECT_EQ(ranges.size(), 1u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 1);
	EXPECT_EQ(it->second, 11);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, addRange7) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (5, 10);
	ranges.addRange (0, 3);
	EXPECT_EQ(ranges.size(), 2u);
	auto it = ranges.begin();
	ASSERT_NE(it, ranges.end());
	EXPECT_EQ(it->first, 0);
	EXPECT_EQ(it->second, 3);
	++it;
	EXPECT_EQ(it->first, 5);
	EXPECT_EQ(it->second, 10);
	++it;
	EXPECT_EQ(it, ranges.end());
}

TEST(NumericRangesTest, valueExists) {
	NumericRanges<int> ranges;
	EXPECT_EQ(ranges.size(), 0u);
	ranges.addRange (8, 15);
	ranges.addRange (0, 3);
	EXPECT_FALSE(ranges.valueExists(-1));
	for (int i=0; i <= 15; ++i)
		EXPECT_NE(ranges.valueExists(i), i > 3 && i < 8) << i;
	EXPECT_FALSE(ranges.valueExists(16));
}


TEST(NumericRangesTest, iterate1) {
	NumericRanges<int> ranges({{3, 9}, {15, 20}});
	auto it = ranges.valueIterator();
	ASSERT_TRUE(it.valid());
	for (int i=3; i <= 9; i++) {
		EXPECT_EQ(*it, i);
		ASSERT_TRUE(it.valid());
		++it;
	}
	for (int i=15; i <= 20; i++) {
		EXPECT_EQ(*it, i);
		ASSERT_TRUE(it.valid());
		++it;
	}
	ASSERT_FALSE(it.valid());
}


TEST(NumericRangesTest, iterate2) {
	NumericRanges<int> ranges({{3, 9}});
	auto it = ranges.valueIterator();
	EXPECT_EQ(*it, 3);
	++it;
	EXPECT_EQ(*it, 4);
	it++;
	EXPECT_EQ(*it, 5);
	EXPECT_EQ(*it++, 5);
	EXPECT_EQ(*it, 6);
}