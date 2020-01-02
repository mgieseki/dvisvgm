/*************************************************************************
** StreamReaderTest.cpp                                                 **
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
#include <fstream>
#include <sstream>
#include <string>
#include "StreamReader.hpp"
#include "XXHashFunction.hpp"

using namespace std;

TEST(StreamReaderTest, readString) {
	string str1 = "dvisvgm converts DVI files to SVG.";
	istringstream iss(str1);
	ASSERT_TRUE(bool(iss));
	StreamReader reader(iss);
	string str2 = reader.readString(iss.str().length());
	EXPECT_EQ(str1, str2);
	iss.clear();
	iss.str(str1);
	str2 = reader.readString();
	EXPECT_EQ(str1, str2);
}


TEST(StreamReaderTest, readString_hashed) {
	string str1 = "dvisvgm converts DVI files to SVG.";
	istringstream iss(str1);
	ASSERT_TRUE(bool(iss));
	StreamReader reader(iss);
	XXH32HashFunction hashfunc;
	string str2 = reader.readString(iss.str().length(), hashfunc);
	EXPECT_EQ(str1, str2);
	EXPECT_EQ(hashfunc.digestString(), "190cc9d2");
	iss.clear();
	iss.str(str1);
	hashfunc.reset();
	str2 = reader.readString(hashfunc, false);
	EXPECT_EQ(hashfunc.digestString(), "190cc9d2");
}


TEST(StreamReaderTest, readUnsigned) {
	string str = "\x01\x02\x03\x04";
	istringstream iss(str);
	StreamReader reader(iss);
	uint32_t val = reader.readUnsigned(4);
	EXPECT_EQ(val, 0x01020304u);
}


TEST(StreamReaderTest, readUnsigned_hashed) {
	string str = "\x01\x02\x03\x04";
	istringstream iss(str);
	StreamReader reader(iss);
	XXH32HashFunction hashfunc;
	uint32_t val = reader.readUnsigned(4, hashfunc);
	EXPECT_EQ(val, 0x01020304u);
	EXPECT_EQ(hashfunc.digestString(), "fe96d19c");
}


TEST(StreamReaderTest, readSigned) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	int32_t val = reader.readSigned(4);
	EXPECT_EQ(val, int32_t(0xffeeddcc));
}


TEST(StreamReaderTest, readSigned_hashed) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	XXH32HashFunction hashfunc;
	int32_t val = reader.readSigned(4, hashfunc);
	EXPECT_EQ(val, int32_t(0xffeeddcc));
	EXPECT_EQ(hashfunc.digestString(), "8baa29bd");
}


TEST(StreamReaderTest, readBytes) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	vector<uint8_t> bytes = reader.readBytes(3);
	EXPECT_EQ(bytes.size(), 3u);
	EXPECT_EQ(bytes[0], 0xff);
	EXPECT_EQ(bytes[1], 0xee);
	EXPECT_EQ(bytes[2], 0xdd);
}


TEST(StreamReaderTest, readBytes_hashed) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	XXH32HashFunction hashfunc;
	vector<uint8_t> bytes = reader.readBytes(3, hashfunc);
	EXPECT_EQ(bytes.size(), 3u);
	EXPECT_EQ(bytes[0], 0xff);
	EXPECT_EQ(bytes[1], 0xee);
	EXPECT_EQ(bytes[2], 0xdd);
	EXPECT_EQ(hashfunc.digestString(), "5eda43a0");
	int byte = reader.readByte(hashfunc);
	EXPECT_EQ(byte, 0xcc);
	EXPECT_EQ(hashfunc.digestString(), "8baa29bd");
}

