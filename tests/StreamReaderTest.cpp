/*************************************************************************
** StreamReaderTest.cpp                                                 **
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
#include <fstream>
#include <sstream>
#include <string>
#include "CRC32.hpp"
#include "StreamReader.hpp"

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


TEST(StreamReaderTest, readStringCRC) {
	string str1 = "dvisvgm converts DVI files to SVG.";
	istringstream iss(str1);
	ASSERT_TRUE(bool(iss));
	StreamReader reader(iss);
	CRC32 crc;
	string str2 = reader.readString(iss.str().length(), crc);
	EXPECT_EQ(str1, str2);
	EXPECT_EQ(crc.get(), 0x7c4ef359u);
	iss.clear();
	iss.str(str1);
	crc.reset();
	str2 = reader.readString(crc, false);
	EXPECT_EQ(crc.get(), 0x7c4ef359u);
}


TEST(StreamReaderTest, readUnsigned) {
	string str = "\x01\x02\x03\x04";
	istringstream iss(str);
	StreamReader reader(iss);
	uint32_t val = reader.readUnsigned(4);
	EXPECT_EQ(val, 0x01020304u);
}


TEST(StreamReaderTest, readUnsignedCRC) {
	string str = "\x01\x02\x03\x04";
	istringstream iss(str);
	StreamReader reader(iss);
	CRC32 crc;
	uint32_t val = reader.readUnsigned(4, crc);
	EXPECT_EQ(val, 0x01020304u);
	EXPECT_EQ(crc.get(), 0xb63cfbcdu);
}


TEST(StreamReaderTest, readSigned) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	int32_t val = reader.readSigned(4);
	EXPECT_EQ(val, int32_t(0xffeeddcc));
}


TEST(StreamReaderTest, readSignedCRC) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	CRC32 crc;
	int32_t val = reader.readSigned(4, crc);
	EXPECT_EQ(val, int32_t(0xffeeddcc));
	EXPECT_EQ(crc.get(), 0xfa79118eu);
}


TEST(StreamReaderTest, readBytes) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	vector<uint8_t> bytes(4);
	memset(&bytes[0], 0, 4);
	reader.readBytes(3, bytes);
	EXPECT_EQ(bytes[0], 0xff);
	EXPECT_EQ(bytes[1], 0xee);
	EXPECT_EQ(bytes[2], 0xdd);
	EXPECT_EQ(bytes[3], 0);
}


TEST(StreamReaderTest, readBytesCRC) {
	string str = "\xff\xee\xdd\xcc";
	istringstream iss(str);
	StreamReader reader(iss);
	vector<uint8_t> bytes(4);
	memset(&bytes[0], 0, 4);
	CRC32 crc;
	reader.readBytes(3, bytes, crc);
	EXPECT_EQ(bytes[0], 0xff);
	EXPECT_EQ(bytes[1], 0xee);
	EXPECT_EQ(bytes[2], 0xdd);
	EXPECT_EQ(bytes[3], 0);
	EXPECT_EQ(crc.get(), 0x68ab9f15u);
	int byte = reader.readByte(crc);
	EXPECT_EQ(byte, 0xcc);
	EXPECT_EQ(crc.get(), 0x2d652e62u);
}

