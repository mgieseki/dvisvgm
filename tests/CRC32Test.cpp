/*************************************************************************
** CRC32Test.cpp                                                        **
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
#include <string>
#include "CRC32.hpp"

using namespace std;

TEST(CRC32Test, init) {
	CRC32 crc;
	EXPECT_EQ(crc.get(), 0u);
	crc.reset();
	EXPECT_EQ(crc.get(), 0u);
}


TEST(CRC32Test, update_numbers) {
	CRC32 crc;
	const uint32_t comp[]={0xf8923be6, 0x4dcd2866, 0x4fdbb57a, 0xcb40f3bf, 0xf1a33887, 0x3422c816, 0x3a8b92a4, 0xedafc4ce, 0x4f5d17ec, 0x4a9f2f68};
	for (int i=-5; i < 5; ++i) {
		crc.update(i);
		EXPECT_EQ(crc.get(), comp[i+5]) << "i=" << i;
	}
	crc.reset();
	EXPECT_EQ(crc.get(), 0u);
	// add 32 bit values
	for (int i=0; i < 256; ++i)
		crc.update(i);
	EXPECT_EQ(crc.get(), 0xd4c2968b);
	for (int i=256; i < 10000; ++i)
		crc.update(i);
	EXPECT_EQ(crc.get(), 0x6de9e841u);
}


TEST(CRC32Test, update_array) {
	CRC32 crc;
	uint8_t vals[256];
	// add 8 bit values
	for (int i=0; i < 256; ++i)
		vals[i] = i;
	crc.update(vals, 256);
	EXPECT_EQ(crc.get(), 0x29058c73u);
}


TEST(CRC32Test, update_string) {
	CRC32 crc;
	crc.update("dvisvgm converts DVI files to SVG.");
	EXPECT_EQ(crc.get(), 0x7c4ef359u);
	crc.update("It is published under the GNU General Public License.");
	EXPECT_EQ(crc.get(), 0xa732081bu);
}


TEST(CRC32Test, compute) {
	EXPECT_EQ(CRC32::compute("dvisvgm converts DVI files to SVG."), 0x7c4ef359u);
	istringstream iss("It is published under the GNU General Public License.");
	EXPECT_EQ(CRC32::compute(iss), 0xbc02ba40u);
}
