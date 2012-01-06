/*************************************************************************
** StreamReaderTest.cpp                                                 **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "CRC32.h"
#include "StreamReader.h"

using namespace std;

TEST(StreamReaderTest, readString) {
	istringstream iss("dvisvgm converts DVI files to SVG.");
	StreamReader reader(iss);
	string str = reader.readString(iss.str().length());
	EXPECT_EQ(str, iss.str());
}


TEST(StreamReaderTest, readStringCRC) {
	istringstream iss("dvisvgm converts DVI files to SVG.");
	StreamReader reader(iss);
	CRC32 crc;
	string str = reader.readString(iss.str().length(), crc);
	EXPECT_EQ(str, iss.str());
	EXPECT_EQ(crc.get(), 0x7c4ef359);
}


TEST(StreamReaderTest, readUnsigned) {
	string str;
	str.push_back('\x00');
	str.push_back('\x01');
	str.push_back('\x02');
	str.push_back('\x03');
	istringstream iss(str);
	StreamReader reader(iss);
	UInt32 val = reader.readUnsigned(4);
	EXPECT_EQ(val, 0x00010203);
}


TEST(StreamReaderTest, readUnsignedCRC) {
	string str;
	str.push_back('\x00');
	str.push_back('\x01');
	str.push_back('\x02');
	str.push_back('\x03');
	istringstream iss(str);
	StreamReader reader(iss);
	CRC32 crc;
	UInt32 val = reader.readUnsigned(4, crc);
	EXPECT_EQ(val, 0x00010203);
	EXPECT_EQ(crc.get(), 0x8bb98613);
}


TEST(StreamReaderTest, readSigned) {
	string str;
	str.push_back('\xff');
	str.push_back('\xee');
	str.push_back('\xdd');
	str.push_back('\xcc');
	istringstream iss(str);
	StreamReader reader(iss);
	Int32 val = reader.readUnsigned(4);
	EXPECT_EQ(val, 0xffeeddcc);
}


TEST(StreamReaderTest, readSignedCRC) {
	string str;
	str.push_back('\xff');
	str.push_back('\xee');
	str.push_back('\xdd');
	str.push_back('\xcc');
	istringstream iss(str);
	StreamReader reader(iss);
	CRC32 crc;
	Int32 val = reader.readUnsigned(4, crc);
	EXPECT_EQ(val, 0xffeeddcc);
	EXPECT_EQ(crc.get(), 0xfa79118e);
}
