/*************************************************************************
** StringMatcherTest.cpp                                                **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2019 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "InputReader.hpp"

using namespace std;

TEST(StringMatcherTest, match1) {
	istringstream iss;
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_FALSE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 0u);
}


TEST(StringMatcherTest, match2) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 1u);
}


TEST(StringMatcherTest, match3) {
	istringstream iss("abcpatatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_FALSE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 15u);
}


TEST(StringMatcherTest, match4) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
	iss.clear();
	iss.str("abcpatpattern");
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
	iss.clear();
	iss.str("pattern");
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 7u);
}


TEST(StringMatcherTest, match5) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
}
