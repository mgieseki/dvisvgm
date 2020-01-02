/*************************************************************************
** ToUnicodeMapTest.cpp                                                 **
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
#include <string>
#include "ToUnicodeMap.hpp"

using namespace std;

#define CHECK_RANGE(name, ucmap, min, max, minval) \
	{SCOPED_TRACE(name); check_range(ucmap, min, max, minval);}


static void check_range (const ToUnicodeMap &ucmap, uint32_t min, uint32_t max, uint32_t minval) {
	for (uint32_t i=min; i <= max; i++)
		ASSERT_EQ(ucmap.valueAt(i), minval+(i-min)) << "i=" << i;
}


TEST(ToUnicodeMapTest, addMissingMappings1) {
	ToUnicodeMap ucmap;
	ASSERT_TRUE(ucmap.addMissingMappings(20));
	ASSERT_EQ(ucmap.numRanges(), 1u);
	CHECK_RANGE("A", ucmap, 1, 20, 1);
}


TEST(ToUnicodeMapTest, addMissingMappings2) {
	ToUnicodeMap ucmap;
	ucmap.addRange(5, 8, 40);
	ASSERT_EQ(ucmap.numRanges(), 1u);
	CHECK_RANGE("A", ucmap, 5, 8, 40);

	ucmap.addRange(10, 15, 50);
	ASSERT_EQ(ucmap.numRanges(), 2u);
	CHECK_RANGE("B", ucmap, 5, 8, 40);
	CHECK_RANGE("C", ucmap, 10, 15, 50);

	ASSERT_TRUE(ucmap.addMissingMappings(20));
	ASSERT_EQ(ucmap.numRanges(), 2u);
	CHECK_RANGE("D", ucmap, 1, 9, 36);
	CHECK_RANGE("E", ucmap, 10, 20, 50);
}
