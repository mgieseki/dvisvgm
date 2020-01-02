/*************************************************************************
** PageRagesTest.cpp                                                    **
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
#include <ostream>
#include "PageRanges.hpp"

using namespace std;


struct Range
{
	int l, r;
};


static bool is_equal (const PageRanges &pr, const Range *r) {
	int i=0;
	for (PageRanges::ConstIterator it=pr.begin(); it != pr.end(); ++it) {
		if (it->first != r[i].l || it->second != r[i].r)
			return false;
		++i;
	}
	return true;
}


TEST(PageRangesTest, single) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("123"));
	ASSERT_FALSE(pr.ranges().empty());
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(123,123));
	ASSERT_EQ(pr.numberOfPages(), 1u);
}


TEST(PageRangesTest, single_range) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("3-19"));
	ASSERT_FALSE(pr.ranges().empty());
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(3,19));
	ASSERT_EQ(pr.numberOfPages(), 17u);
}


TEST(PageRangesTest, single_range_inv) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("19-3"));
	ASSERT_FALSE(pr.ranges().empty());
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(3,19));
	ASSERT_EQ(pr.numberOfPages(), 17u);
}


TEST(PageRangesTest, single_range_lopen) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("-19"));
	ASSERT_FALSE(pr.ranges().empty());
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(1,19));
	ASSERT_EQ(pr.numberOfPages(), 19u);
}


TEST(PageRangesTest, single_range_ropen) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("4-", 100));
	ASSERT_FALSE(pr.ranges().empty());
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(4,100));
	ASSERT_EQ(pr.numberOfPages(), 97u);
}


TEST(PageRangesTest, multiple1) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("11,6,9,1,75,6,3"));
	ASSERT_EQ(pr.size(), 6u);
	Range cmp[] = {{1,1},{3,3},{6,6},{9,9},{11,11},{75,75}};
	ASSERT_TRUE(is_equal(pr, cmp));
	ASSERT_EQ(pr.numberOfPages(), 6u);
}


TEST(PageRangesTest, multiple2) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("2,6,9,1,75,6,3"));
	ASSERT_EQ(pr.size(), 4u);
	Range cmp[] = {{1,3},{6,6},{9,9},{75,75}};
	ASSERT_TRUE(is_equal(pr, cmp));
}


TEST(PageRangesTest, multiple3) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("2,6,9-11,1,25-,19-13,6,3", 100));
	ASSERT_EQ(pr.size(), 5u);
	Range cmp[] = {{1,3},{6,6},{9,11},{13,19},{25,100}};
	ASSERT_TRUE(is_equal(pr, cmp));
}


TEST(PageRangesTest, overlap1) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("5-11, 8-15"));
	ASSERT_EQ(pr.size(), 1u);
	Range cmp[] = {{5,15}};
	ASSERT_TRUE(is_equal(pr, cmp));
}


TEST(PageRangesTest, overlap2) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("23-30, 5-11, 17-23, 12-19"));
	ASSERT_EQ(pr.size(), 1u);
	Range cmp[] = {{5,30}};
	ASSERT_TRUE(is_equal(pr, cmp));
}


TEST(PageRangesTest, overlap3) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("22-30, 5-11, 21-23, 12-19"));
	ASSERT_EQ(pr.size(), 2u);
	Range cmp[] = {{5,19},{21,30}};
	ASSERT_TRUE(is_equal(pr, cmp));
}


TEST(PageRangesTest, overlap4) {
	PageRanges pr;
	ASSERT_TRUE(pr.parse("-15, 20-, 8, 12-16, 18-19", 100));
	ASSERT_EQ(pr.size(), 2u);
	Range cmp[] = {{1,16},{18,100}};
	ASSERT_TRUE(is_equal(pr, cmp));
	ASSERT_TRUE(pr.parse("17"));
	ASSERT_EQ(pr.size(), 1u);
	ASSERT_EQ(pr.ranges().front(), PageRanges::Range(1,100));
}


TEST(PageRangesTest, error) {
	PageRanges pr;
	ASSERT_FALSE(pr.parse("x"));
	ASSERT_FALSE(pr.parse("5-x"));
	ASSERT_FALSE(pr.parse("5 6"));
	ASSERT_FALSE(pr.parse("5,"));
}
