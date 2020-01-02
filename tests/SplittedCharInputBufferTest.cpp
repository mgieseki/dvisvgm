/*************************************************************************
** SplittedCharInputBufferTest.cpp                                      **
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
#include <cstring>
#include <sstream>
#include "InputBuffer.hpp"
#include "InputReader.hpp"

using std::istringstream;

TEST(SplittedCharInputBufferTest, get) {
	const char *buf1 = "abcdef";
	const char *buf2 = "ghijklmnopqrstuvwxyz";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	bool ok=true;
	for (int i=0; !in.eof() && ok; i++) {
		EXPECT_LT(i, 26);
		EXPECT_EQ(in.get(), 'a'+i);
		ok = (i < 26);
	}
}


TEST(SplittedCharInputBufferTest, peek) {
	const char *buf1 = "abcdef";
	const char *buf2 = "ghijklmnopqrstuvwxyz";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	EXPECT_EQ(in.peek(), 'a');
	for (int i=0; i < 26; i++)
		EXPECT_EQ(in.peek(i), 'a'+i);
	EXPECT_EQ(in.peek(26), -1);
}


TEST(SplittedCharInputBufferTest, check) {
	const char *buf1 = "abcde";
	const char *buf2 = "fghijklmnopqrstuvwxyz";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	EXPECT_TRUE(in.check("abc", false));
	EXPECT_TRUE(in.check("abc", true));
	EXPECT_TRUE(in.check("def", true));
	EXPECT_TRUE(in.check("ghi", true));
	EXPECT_TRUE(in.check("jkl", true));
	EXPECT_TRUE(in.check("mnopqrst", false));
	EXPECT_TRUE(in.check("mnopqrst", true));
	EXPECT_TRUE(in.check("uvwxyz", true));
	EXPECT_EQ(in.get(), -1);
}


TEST(SplittedCharInputBufferTest, skip) {
	const char *buf1 = "abcde";
	const char *buf2 = "fghijklmnopqrstuvwxyz";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	in.skip(3);
	EXPECT_EQ(in.peek(), 'd');
	in.skipUntil("ijk");
	EXPECT_EQ(in.peek(), 'l');
	in.skipUntil("z");
	EXPECT_TRUE(in.eof());
}


TEST(SplittedCharInputBufferTest, parseInt) {
	const char *buf1 = "1234,-";
	const char *buf2 = "5,+6,10.-";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	int n;
	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 1234);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, -5);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 6);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseInt(n));
	EXPECT_EQ(n, 10);
	EXPECT_EQ(in.get(), '.');

	EXPECT_FALSE(in.parseInt(n));
	EXPECT_EQ(in.get(), '-');
}


TEST(SplittedCharInputBufferTest, parseUInt_base) {
	const char *buf1 = "1234,-5,10,1a";
	const char *buf2 = "bc,1234a";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	unsigned n;
	EXPECT_TRUE(in.parseUInt(10, n));
	EXPECT_EQ(n, 1234u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_FALSE(in.parseUInt(10, n));
	in.get();
	EXPECT_TRUE(in.parseUInt(10, n));
	EXPECT_EQ(n, 5u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(16, n));
	EXPECT_EQ(n, 16u);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(16, n));
	EXPECT_EQ(n, 0x1ABCu);
	EXPECT_EQ(in.get(), ',');

	EXPECT_TRUE(in.parseUInt(8, n));
	EXPECT_EQ(n, 01234u);
	EXPECT_EQ(in.get(), 'a');
}


TEST(SplittedCharInputBufferTest, parseDouble) {
	const char *buf1 = "1234,-5,6.12,-3";
	const char *buf2 = ".1415,-.1,12e2,10.-";
	SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
	BufferInputReader in(buffer);
	double d;
	EXPECT_EQ(in.parseDouble(d), 'i');
	EXPECT_EQ(d, 1234.0);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'i');
	EXPECT_EQ(d, -5.0);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 6.12);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, -3.1415);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, -0.1);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 1200);
	EXPECT_EQ(in.get(), ',');

	EXPECT_EQ(in.parseDouble(d), 'f');
	EXPECT_EQ(d, 10.0);
	EXPECT_EQ(in.peek(), '-');

	EXPECT_FALSE(in.parseDouble(d));
	EXPECT_EQ(in.get(), '-');
}
