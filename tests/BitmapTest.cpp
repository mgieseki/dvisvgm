/*************************************************************************
** BitmapTest.cpp                                                       **
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
#include "Bitmap.hpp"

using namespace std;


TEST(BitmapTest, bbox) {
	Bitmap bitmap(-4, 10, 3, 10);
	EXPECT_EQ(bitmap.width(), 15);
	EXPECT_EQ(bitmap.height(), 8);
	EXPECT_EQ(bitmap.xshift(), -4);
	EXPECT_EQ(bitmap.yshift(), 3);
	EXPECT_EQ(bitmap.bytesPerRow(), 2);
	EXPECT_EQ(bitmap.empty(), false);
}


TEST(BitmapTest, setBits) {
	int w, h;
	Bitmap bitmap(-4, 10, 3, 10);
	bitmap.getExtent(w, h);
	EXPECT_EQ(w, 0);
	EXPECT_EQ(h, 0);
	bitmap.setBits(3, 5, 5);
	bitmap.getExtent(w, h);
	EXPECT_EQ(w, 5);
	EXPECT_EQ(h, 1);

	EXPECT_EQ(bitmap.rowPtr(3-3)[0], 0);
	EXPECT_EQ(bitmap.rowPtr(3-3)[1], 0x7c);

	EXPECT_EQ(bitmap.rowPtr(10-3)[0], 0);
	EXPECT_EQ(bitmap.rowPtr(10-3)[1], 0);
	bitmap.setBits(10, -3, 10);
	bitmap.getExtent(w, h);
	EXPECT_EQ(w, 13);
	EXPECT_EQ(h, 8);

	EXPECT_EQ(bitmap.rowPtr(10-3)[0], 0x7f);
	EXPECT_EQ(bitmap.rowPtr(10-3)[1], 0xe0);
}


TEST(BitmapTest, copy) {
	Bitmap bitmap(-4, 10, 3, 10);
	vector<uint16_t> target;
	bitmap.setBits(4, 5, 5);
	bitmap.copy(target, false);
	EXPECT_EQ(target.size(), 8u);
	EXPECT_EQ(target[0], 0);
	EXPECT_EQ(target[1], 124);
	EXPECT_EQ(target[2], 0);
	bitmap.copy(target, true);
	EXPECT_EQ(target.size(), 8u);
	EXPECT_EQ(target[5], 0);
	EXPECT_EQ(target[6], 124);
	EXPECT_EQ(target[7], 0);
}
