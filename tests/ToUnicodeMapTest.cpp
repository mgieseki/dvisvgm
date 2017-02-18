/*************************************************************************
** ToUnicodeMapTest.cpp                                                 **
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
#include <sstream>
#include <string>
#include "ToUnicodeMap.hpp"

using namespace std;

#define CHECK_RANGE(ucmap, min, max, minval) check_range(ucmap, min, max, minval, __LINE__)

static void check_range (const ToUnicodeMap &ucmap, uint32_t min, uint32_t max, uint32_t minval, int line) {
	for (uint32_t i=min; i <= max; i++)
		ASSERT_EQ(ucmap.valueAt(i), minval+(i-min)) << __FILE__ << ":" << line << ": i=" << i;
}


TEST(ToUnicodeMapTest, addMissingMappings1) {
	ToUnicodeMap ucmap;
	ASSERT_TRUE(ucmap.addMissingMappings(20));
	ASSERT_EQ(ucmap.size(), 1);
	CHECK_RANGE(ucmap, 1, 20, 1);
}


TEST(ToUnicodeMapTest, addMissingMappings2) {
	ToUnicodeMap ucmap;
	ucmap.addRange(5, 8, 40);
	ASSERT_EQ(ucmap.size(), 1);
	CHECK_RANGE(ucmap, 5, 8, 40);

	ucmap.addRange(10, 15, 50);
	ASSERT_EQ(ucmap.size(), 2);
	CHECK_RANGE(ucmap, 5, 8, 40);
	CHECK_RANGE(ucmap, 10, 15, 50);

	ASSERT_TRUE(ucmap.addMissingMappings(20));
	ASSERT_EQ(ucmap.size(), 2);
	CHECK_RANGE(ucmap, 1, 9, 36);
	CHECK_RANGE(ucmap, 10, 20, 50);
}
