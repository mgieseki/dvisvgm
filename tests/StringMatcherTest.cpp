/*************************************************************************
** StringMatcherTest.cpp                                                **
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
#include "InputReader.hpp"

using namespace std;

TEST(StringMatcherTest, match1) {
	istringstream iss;
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_FALSE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 0u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, match2) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 1u);
	EXPECT_EQ(ir.peek(), 'b');
}


TEST(StringMatcherTest, match3) {
	istringstream iss("abcpatatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_FALSE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 15u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, match4) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_EQ(ir.peek(), 'x');
	iss.clear();
	iss.str("abcpatpattern");
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_LT(ir.peek(), 0);
	iss.clear();
	iss.str("pattern");
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 7u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, match5) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.match(ir));
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_EQ(ir.peek(), 'x');
}


TEST(StringMatcherTest, read1) {
	istringstream iss;
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_TRUE(matcher.read(ir).empty());
	EXPECT_EQ(matcher.charsRead(), 0u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, read2) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("");
	StreamInputReader ir(iss);
	EXPECT_EQ(matcher.read(ir), "a");
	EXPECT_EQ(matcher.charsRead(), 1u);
	EXPECT_EQ(ir.peek(), 'b');
}


TEST(StringMatcherTest, read3) {
	istringstream iss("abcpatatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_EQ(matcher.read(ir), "abcpatatternxyz");
	EXPECT_EQ(matcher.charsRead(), 15u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, read4) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_EQ(matcher.read(ir), "abcpatpattern");
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_EQ(ir.peek(), 'x');
	iss.clear();
	iss.str("abcpatpattern");
	EXPECT_EQ(matcher.read(ir), "abcpatpattern");
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_LT(ir.peek(), 0);
	iss.clear();
	iss.str("pattern");
	EXPECT_EQ(matcher.read(ir), "pattern");
	EXPECT_EQ(matcher.charsRead(), 7u);
	EXPECT_LT(ir.peek(), 0);
}


TEST(StringMatcherTest, read5) {
	istringstream iss("abcpatpatternxyz");
	StringMatcher matcher("pattern");
	StreamInputReader ir(iss);
	EXPECT_EQ(matcher.read(ir), "abcpatpattern");
	EXPECT_EQ(matcher.charsRead(), 13u);
	EXPECT_EQ(ir.peek(), 'x');
}
