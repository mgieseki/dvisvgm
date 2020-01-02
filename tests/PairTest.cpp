/*************************************************************************
** PairTest.cpp                                                         **
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
#include <sstream>
#include "Pair.hpp"

using namespace std;

TEST(PairTest, constructor) {
	DPair p(-1.5, 2);
	ASSERT_EQ(p.x(), -1.5);
	ASSERT_EQ(p.y(), 2);
	DPair q = p;
	ASSERT_EQ(q.x(), -1.5);
	ASSERT_EQ(q.y(), 2);
}


TEST(PairTest, setter) {
	DPair p(1,2);
	ASSERT_EQ(p, DPair(1,2));
	p.x(3);
	ASSERT_EQ(p, DPair(3,2));
	p.y(5);
	ASSERT_EQ(p, DPair(3,5));
}


TEST(PairTest, length) {
	ASSERT_EQ(DPair(0,0).length(), 0);
	ASSERT_EQ(DPair(1,0).length(), 1);
	ASSERT_EQ(DPair(0,1).length(), 1);
	ASSERT_EQ(DPair(-1,0).length(), 1);
	ASSERT_EQ(DPair(0,-1).length(), 1);

	// check some Pythagorean triples
	for (int i=1; i < 10; ++i) {
		for (int j=i+1; j <= 10; ++j) {
			int a = j*j-i*i;
			int b = 2*i*j;
			int c = j*j+i*i;
			ASSERT_EQ(DPair(a, b).length(), c);
			ASSERT_EQ(DPair(b, a).length(), c);
			ASSERT_EQ(DPair(-a, b).length(), c);
			ASSERT_EQ(DPair(-b, a).length(), c);
			ASSERT_EQ(DPair(-a, -b).length(), c);
		}
	}
}


TEST(PairTest, add) {
	ASSERT_EQ(DPair(2,3)+DPair(0,0), DPair(2,3));
	ASSERT_EQ(DPair(2,3)+DPair(4,5), DPair(6,8));
	ASSERT_EQ(DPair(2,3)+DPair(-4,-5), DPair(-2,-2));
}


TEST(PairTest, sub) {
	ASSERT_EQ(DPair(2,3)-DPair(0,0), DPair(2,3));
	ASSERT_EQ(DPair(2,3)-DPair(4,5), DPair(-2,-2));
	ASSERT_EQ(DPair(2,3)-DPair(-4,-5), DPair(6,8));
}


TEST(PairTest, mul) {
	ASSERT_EQ(DPair(2,3)*0.0, DPair(0,0));
	ASSERT_EQ(DPair(2,3)*1.0, DPair(2,3));
	ASSERT_EQ(DPair(2,3)*2.0, DPair(4,6));
	ASSERT_EQ(DPair(2,3)*(-2.0), DPair(-4,-6));
}


TEST(PairTest, div) {
	ASSERT_EQ(DPair(2,3)/1.0, DPair(2,3));
	ASSERT_EQ(DPair(2,3)/2.0, DPair(1,1.5));
	ASSERT_EQ(DPair(2,3)/(-2.0), DPair(-1,-1.5));
}


TEST(PairTest, neg) {
	EXPECT_EQ(-DPair(0, 0), DPair(0, 0));
	EXPECT_EQ(-DPair(1, 2), DPair(-1, -2));
	EXPECT_EQ(-DPair(-1, -2), DPair(1, 2));
	EXPECT_EQ(-DPair(-1, 2), DPair(1, -2));
	EXPECT_EQ(-DPair(1, -2), DPair(-1, 2));
}


TEST(PairTest, abs) {
	EXPECT_EQ(abs(DPair(0, 0)), DPair(0, 0));
	EXPECT_EQ(abs(DPair(1, 2)), DPair(1, 2));
	EXPECT_EQ(abs(DPair(-1, -2)), DPair(1, 2));
	EXPECT_EQ(abs(DPair(-1, 2)), DPair(1, 2));
	EXPECT_EQ(abs(DPair(1, -2)), DPair(1, 2));
}


TEST(PairTest, equality) {
	ASSERT_TRUE(DPair(2,3) == DPair(2,3));
	ASSERT_FALSE(DPair(2,3) == DPair(-2,-3));
	ASSERT_TRUE(DPair(2,3)+DPair(4,5) == DPair(6,8));
	ASSERT_FALSE(DPair(2,3)+DPair(4,5) != DPair(6,8));
}


TEST(PairTest, ortho) {
	ASSERT_EQ(DPair(0,0).ortho(), DPair(0,0));
	ASSERT_EQ(DPair(1,0).ortho(), DPair(0,1));
	ASSERT_EQ(DPair(0,1).ortho(), DPair(-1,0));
	ASSERT_EQ(DPair(-1,0).ortho(), DPair(0,-1));
	ASSERT_EQ(DPair(0,-1).ortho(), DPair(1,0));
}


TEST(PairTest, write) {
	ostringstream oss;
	DPair p(3,4);
	oss << p;
	ASSERT_EQ(oss.str(), "(3,4)");
	p *= 2;
	oss.str("");
	oss << p;
	ASSERT_EQ(oss.str(), "(6,8)");
}
