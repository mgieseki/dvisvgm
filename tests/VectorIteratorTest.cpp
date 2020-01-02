/*************************************************************************
** VectorIteratorTest.cpp                                               **
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
#include "VectorIterator.hpp"

using namespace std;


TEST(VectorIteratorTest, inc) {
	vector<int> vec{4,3,2,9,8,7,10};
	VectorIterator<int> it(vec);
	EXPECT_TRUE(it.valid());
	EXPECT_EQ(*it++, 4);
	EXPECT_EQ(*it, 3);
	EXPECT_EQ(*++it, 2);
	EXPECT_TRUE(it.valid());
	for (size_t i=2; it.valid(); i++) {
		EXPECT_LT(i, vec.size()) << "i=" << i;
		EXPECT_EQ(*it, vec[i]) << "*it=" << *it << ", vec[" << i << "]=" << vec[i];
		++it;
	}
}


TEST(VectorIteratorTest, addsub) {
	vector<int> vec{4,3,2,9,8,7,10};
	VectorIterator<int> it(vec);
	EXPECT_EQ(*(it += 4), 8);
	EXPECT_EQ(*(it -= 3), 3);
	EXPECT_EQ(*(it + 4), 7);
	EXPECT_EQ(*(it - 1), 4);
}


TEST(VectorIteratorTest, dec) {
	vector<int> vec{4,3,2,9,8,7,10};
	VectorIterator<int> it(vec);
	it += 6;
	EXPECT_EQ(*it, 10);
	EXPECT_EQ(*it--, 10);
	EXPECT_EQ(*it, 7);
	EXPECT_EQ(*--it, 8);
	EXPECT_TRUE(it.valid());
	for (size_t i=4; it.valid(); i--) {
		EXPECT_LT(i, vec.size()) << "i=" << i;
		EXPECT_EQ(*it, vec[i]) << "*it=" << *it << ", vec[" << i << "]=" << vec[i];
		--it;
	}
}


TEST(VectorIteratorTest, invalidate) {
	vector<int> vec{4,3,2,9,8,7,10};
	VectorIterator<int> it(vec);
	--it;
	EXPECT_FALSE(it.valid());
	++it;
	EXPECT_TRUE(it.valid());
	it += 10;
	EXPECT_FALSE(it.valid());
	it -= 10;
	EXPECT_TRUE(it.valid());
	it.invalidate();
	EXPECT_FALSE(it.valid());
	it.reset();
	ASSERT_TRUE(it.valid());
	EXPECT_EQ(*it, 4);
}


TEST(VectorIteratorTest, compare) {
	vector<int> vec{4,3,2,9,8,7,10};
	VectorIterator<int> it1(vec);
	VectorIterator<int> it2 = it1 + 1;
	EXPECT_NE(it1, it2);
	++it1;
	EXPECT_EQ(it1, it2);
	EXPECT_LE(it1, it2);
	EXPECT_GE(it1, it2);
	++it2;
	EXPECT_LT(it1, it2);
	EXPECT_GT(it2, it1);
}


struct MyPair {
	int number;
	const char *str;
}
mypair[] = {
	{3, "text"},
	{5, "string"}
};

TEST(VectorIteratorTest, deref) {
	vector<MyPair> vec(mypair, mypair+2);
	VectorIterator<MyPair> it(vec);
	EXPECT_TRUE(it.valid());
	EXPECT_EQ((*it).number, 3);
	EXPECT_EQ((*it).str, mypair[0].str);
	EXPECT_EQ(it->number, 3);
	EXPECT_EQ(it->str, mypair[0].str);
	++it;
	EXPECT_EQ(it->number, 5);
	EXPECT_EQ(it->str, mypair[1].str);
}


TEST(VectorIteratorTest, fail) {
	vector<MyPair> vec(mypair, mypair+2);
	VectorIterator<MyPair> it(vec);
	--it;
	EXPECT_FALSE(it.valid());
	EXPECT_THROW(*it, IteratorException);
	EXPECT_THROW(it->number, IteratorException);
}
