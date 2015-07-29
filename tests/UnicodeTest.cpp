/*************************************************************************
** UnicodeTest.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "Unicode.h"

using namespace std;


TEST(UnicodeTest, isValidCodepoint) {
	for (UInt32 i=0; i <= 0x20; i++)
		EXPECT_FALSE(Unicode::isValidCodepoint(i));
	for (UInt32 i=0x21; i <= 0x7e; i++)
		EXPECT_TRUE(Unicode::isValidCodepoint(i));
	EXPECT_FALSE(Unicode::isValidCodepoint(0xfffe));
	EXPECT_FALSE(Unicode::isValidCodepoint(0xffff));
	EXPECT_TRUE(Unicode::isValidCodepoint(0x10000));
}


TEST(UnicodeTest, utf8) {
	EXPECT_EQ(Unicode::utf8(0x1), string("\x01"));
	EXPECT_EQ(Unicode::utf8(0x47), string("\x47"));
	EXPECT_EQ(Unicode::utf8(0x7f), string("\x7f"));
	EXPECT_EQ(Unicode::utf8(0x80), string("\xc2\x80"));
	EXPECT_EQ(Unicode::utf8(0x07ff), string("\xdf\xbf"));
	EXPECT_EQ(Unicode::utf8(0x0800), string("\xe0\xa0\x80"));
	EXPECT_EQ(Unicode::utf8(0x7fff), string("\xe7\xbf\xbf"));
	EXPECT_EQ(Unicode::utf8(0xfffe), string("\xef\xbf\xbe"));
	EXPECT_EQ(Unicode::utf8(0xffff), string("\xef\xbf\xbf"));
	EXPECT_EQ(Unicode::utf8(0x10000), string("\xf0\x90\x80\x80"));
	EXPECT_EQ(Unicode::utf8(0x10ffff), string("\xf4\x8f\xbf\xbf"));
	EXPECT_TRUE(Unicode::utf8(0x110000).empty());
}


TEST(UnicodeTest, psName2Codepoint) {
	EXPECT_EQ(Unicode::psName2Codepoint("does not exist"), 0);
	EXPECT_EQ(Unicode::psName2Codepoint("Eogonek"), 0x118);
	EXPECT_EQ(Unicode::psName2Codepoint("alpha"), 0x03b1);
	EXPECT_EQ(Unicode::psName2Codepoint("Alpha"), 0x0391);
	EXPECT_EQ(Unicode::psName2Codepoint("alphatonos"), 0x03ac);
	EXPECT_EQ(Unicode::psName2Codepoint("SF460000"), 0x2568);
}
