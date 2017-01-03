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
#include "ToUnicodeMap.hpp"

using namespace std;


static void check_range (const ToUnicodeMap &ucmap, int min, int max, int minval) {
	for (int i=min; i <= max; i++)
		ASSERT_EQ(ucmap.valueAt(i), minval+(i-min));
}


TEST(ToUnicodeMapTest, addMissingMappings1) {
	ToUnicodeMap ucmap;
	bool success = ucmap.addMissingMappings(20);
	ASSERT_TRUE(success);
	ASSERT_EQ(ucmap.size(), 1);
	check_range(ucmap, 1, 20, 1);
}


TEST(ToUnicodeMapTest, addMissingMappings2) {
	ToUnicodeMap ucmap;
	ucmap.addRange(5, 8, 40);
	ASSERT_EQ(ucmap.size(), 1);
	check_range(ucmap, 5, 8, 40);

	ucmap.addRange(10, 15, 50);
	ASSERT_EQ(ucmap.size(), 2);
	check_range(ucmap, 5, 8, 40);
	check_range(ucmap, 10, 15, 50);

	bool success = ucmap.addMissingMappings(20);
	ASSERT_TRUE(success);
	ASSERT_EQ(ucmap.size(), 2);
	check_range(ucmap, 1, 9, 36);
	check_range(ucmap, 10, 20, 50);
}


