/*************************************************************************
** TypesTest.cpp                                                        **
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

#include <gtest/gtest.h>
#include "types.h"

using namespace std;


TEST(TypesTest, size_signed) {
	ASSERT_EQ(sizeof(Int8),  1);
	ASSERT_EQ(sizeof(Int16), 2);
	ASSERT_EQ(sizeof(Int32), 4);
	ASSERT_EQ(sizeof(Int64), 8);
}


TEST(TypesTest, neg_signed) {
	ASSERT_LT(Int8(-1),  0);
	ASSERT_LT(Int16(-1), 0);
	ASSERT_LT(Int32(-1), 0);
	ASSERT_LT(Int64(-1), 0);
}


TEST(TypesTest, unsigned_types) {
	ASSERT_EQ(sizeof(UInt8),  1);
	ASSERT_EQ(sizeof(UInt16), 2);
	ASSERT_EQ(sizeof(UInt32), 4);
	ASSERT_EQ(sizeof(UInt64), 8);
}


TEST(TypesTest, neg_unsigned) {
	ASSERT_GT(UInt8(-1),  0);
	ASSERT_GT(UInt16(-1), 0);
	ASSERT_GT(UInt32(-1), 0);
	ASSERT_GT(UInt64(-1), 0);
}

