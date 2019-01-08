/*************************************************************************
** StreamWriterTest.cpp                                                 **
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
#include <string>
#include "CRC32.hpp"
#include "StreamWriter.hpp"

using namespace std;

TEST(StreamWriterTest, writeString) {
	ostringstream oss;
	StreamWriter writer(oss);
	string str = "dvisvgm converts DVI files to SVG.";
	writer.writeString(str);
	EXPECT_EQ(oss.str(), str);

	oss.str("");
	writer.writeString(str, true);
	str.push_back('\0');
	EXPECT_EQ(oss.str(), str);
}


TEST(StreamWriterTest, writeStringCRC) {
	ostringstream oss;
	StreamWriter writer(oss);
	CRC32 crc;
	string str = "dvisvgm converts DVI files to SVG.";
	writer.writeString(str, crc);
	EXPECT_EQ(oss.str(), str);
	EXPECT_EQ(crc.get(), 0x7C4EF359u);

	oss.str("");
	crc.reset();
	writer.writeString(str, crc, true);
	str.push_back('\0');
	EXPECT_EQ(oss.str(), str);
	EXPECT_EQ(crc.get(), 0xc0c9482e);
}


TEST(StreamWriterTest, writeUnsigned) {
	ostringstream oss;
	StreamWriter writer(oss);
	writer.writeUnsigned(0x00010203, 4);
	string str;
	str.push_back('\x00');
	str.push_back('\x01');
	str.push_back('\x02');
	str.push_back('\x03');
	EXPECT_EQ(oss.str(), str);
}


TEST(StreamWriterTest, writeUnsignedCRC) {
	ostringstream oss;
	StreamWriter writer(oss);
	CRC32 crc;
	writer.writeUnsigned(0x00010203, 4, crc);
	string str;
	str.push_back('\x00');
	str.push_back('\x01');
	str.push_back('\x02');
	str.push_back('\x03');
	EXPECT_EQ(oss.str(), str);
	EXPECT_EQ(crc.get(), 0x8bb98613);
}


TEST(StreamWriterTest, writeSigned) {
	ostringstream oss;
	StreamWriter writer(oss);
	writer.writeSigned(0xffeeddcc, 4);
	string str = "\xff\xee\xdd\xcc";
	EXPECT_EQ(oss.str(), str);
}


TEST(StreamWriterTest, writeSignedCRC) {
	ostringstream oss;
	StreamWriter writer(oss);
	CRC32 crc;
	writer.writeSigned(0xffeeddcc, 4, crc);
	string str = "\xff\xee\xdd\xcc";
	EXPECT_EQ(oss.str(), str);
	EXPECT_EQ(crc.get(), 0xfa79118e);
}
