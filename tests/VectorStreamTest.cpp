/*************************************************************************
** VectorStreamTest.cpp                                                 **
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
#include <vector>
#include "VectorStream.hpp"

using std::string;
using std::vector;

TEST(VectorStreamTest, read1) {
	const char *str = "abcdefghijklm\0nopqrstuvwxyz";
	vector<char> vec(str, str+27);
	VectorInputStream<char> vs(vec);
	for (unsigned count = 0; vs; count++) {
		int c = vs.get();
		if (count < vec.size()) {
			EXPECT_EQ(c, str[count]) << "count=" << count;
		}
		else {
			EXPECT_EQ(c, -1);
		}
	}
}


TEST(VectorStreamTest, read2) {
	vector<int> vec;
	VectorInputStream<int> vs(vec);
	EXPECT_EQ(vs.get(), -1);
}


TEST(VectorStreamTest, read3) {
	vector<int> vec;
	vec.push_back(-2);
	vec.push_back(-1);
	vec.push_back(0);
	vec.push_back(1);
	VectorInputStream<int> vs(vec);
	EXPECT_EQ(vs.get(), 254);
	EXPECT_EQ(vs.get(), 255);
	EXPECT_EQ(vs.get(), 0);
	EXPECT_EQ(vs.get(), 1);
	EXPECT_EQ(vs.get(), -1);
}
