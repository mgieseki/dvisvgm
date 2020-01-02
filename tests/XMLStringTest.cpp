/*************************************************************************
** XMLStringTest.cpp                                                    **
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
#include "XMLString.hpp"

using namespace std;


TEST(XMLStringTest, stringcast) {
	const char *cstr = "special characters: &\"'<>";
	string str = cstr;
	EXPECT_EQ(XMLString(cstr, true), string("special characters: &\"'<>"));
	EXPECT_EQ(XMLString(cstr), string("special characters: &amp;&quot;&apos;&lt;>"));
	EXPECT_EQ(XMLString(str, true), string("special characters: &\"'<>"));
	EXPECT_EQ(XMLString(str), string("special characters: &amp;&quot;&apos;&lt;>"));
}


TEST(XMLStringTest, intcast) {
	EXPECT_EQ(XMLString(65, true), string("65"));
	EXPECT_EQ(XMLString(65, false), string("A"));
	EXPECT_EQ(XMLString(1000, true), string("1000"));
	EXPECT_EQ(XMLString(1000, false), string("\xCF\xA8"));  // UTF-8
}


TEST(XMLStringTest, doublecast) {
	EXPECT_EQ(XMLString(1.2345), string("1.2345"));
	XMLString::DECIMAL_PLACES = 3;
	EXPECT_EQ(XMLString(1.2345), string("1.235"));
	XMLString::DECIMAL_PLACES = 2;
	EXPECT_EQ(XMLString(1.2345), string("1.23"));
	XMLString::DECIMAL_PLACES = 1;
	EXPECT_EQ(XMLString(1.2345), string("1.2"));
	XMLString::DECIMAL_PLACES = 0;
	EXPECT_EQ(XMLString(1.2345), string("1.2345"));

	EXPECT_EQ(XMLString(-1.2345), string("-1.2345"));
	XMLString::DECIMAL_PLACES = 3;
	EXPECT_EQ(XMLString(-1.2345), string("-1.235"));
	XMLString::DECIMAL_PLACES = 2;
	EXPECT_EQ(XMLString(-1.2345), string("-1.23"));
	XMLString::DECIMAL_PLACES = 1;
	EXPECT_EQ(XMLString(-1.2345), string("-1.2"));
	XMLString::DECIMAL_PLACES = 0;
	EXPECT_EQ(XMLString(-1.2345), string("-1.2345"));

	EXPECT_EQ(XMLString(10.0), string("10"));
	EXPECT_EQ(XMLString(-10.0), string("-10"));
}
