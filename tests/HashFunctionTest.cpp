/*************************************************************************
** HashFunctionTest.cpp                                                 **
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
#include "MD5HashFunction.hpp"
#include "XXHashFunction.hpp"

using namespace std;


TEST(HashFunctionTest, md5) {
	MD5HashFunction md5;
	ASSERT_EQ(md5.digestSize(), 16);
	md5.update("0123456789");
	EXPECT_EQ(md5.digestString(), "781e5e245d69b566979b86e28d23f2c7");
	md5.update("abcdefghij");
	EXPECT_EQ(md5.digestString(), "644be06dfc54061fd1e67f5ebbabcd58");
	md5.reset();
	md5.update("0123456789");
	EXPECT_EQ(md5.digestString(), "781e5e245d69b566979b86e28d23f2c7");
	vector<uint8_t> bytes = {0x78, 0x1e, 0x5e, 0x24, 0x5d, 0x69, 0xb5, 0x66, 0x97, 0x9b, 0x86, 0xe2, 0x8d, 0x23, 0xf2, 0xc7};
	EXPECT_EQ(md5.digestValue(), bytes);
}


TEST(HashFunctionTest, xxh32) {
	XXH32HashFunction xxh32;
	ASSERT_EQ(xxh32.digestSize(), 4);
	xxh32.update("0123456789");
	EXPECT_EQ(xxh32.digestString(), "950c9c0a");
	xxh32.update("abcdefghij");
	EXPECT_EQ(xxh32.digestString(), "35600916");
	xxh32.reset();
	xxh32.update("0123456789");
	EXPECT_EQ(xxh32.digestString(), "950c9c0a");
	vector<uint8_t> bytes = {0x95, 0x0c, 0x9c, 0x0a};
	EXPECT_EQ(xxh32.digestValue(), bytes);
}


TEST(HashFunctionTest, xxh64) {
	XXH64HashFunction xxh64;
	ASSERT_EQ(xxh64.digestSize(), 8);
	xxh64.update("0123456789");
	EXPECT_EQ(xxh64.digestString(), "3f5fc178a81867e7");
	xxh64.update("abcdefghij");
	EXPECT_EQ(xxh64.digestString(), "45c2c0e1eb35a0b6");
	xxh64.reset();
	xxh64.update("0123456789");
	EXPECT_EQ(xxh64.digestString(), "3f5fc178a81867e7");
	vector<uint8_t> bytes = {0x3f, 0x5f, 0xc1, 0x78, 0xa8, 0x18, 0x67, 0xe7};
	EXPECT_EQ(xxh64.digestValue(), bytes);
}


#ifdef ENABLE_XXH128
TEST(HashFunctionTest, xxh128) {
	XXH128HashFunction xxh128;
	ASSERT_EQ(xxh128.digestSize(), 16);
	xxh128.update("0123456789");
	EXPECT_EQ(xxh128.digestString(), "e353667619ec664b49655fc9692165fb");
	xxh128.update("abcdefghij");
	EXPECT_EQ(xxh128.digestString(), "5f64782bdc987c162389e97fd11aed64");
	xxh128.reset();
	xxh128.update("0123456789");
	EXPECT_EQ(xxh128.digestString(), "e353667619ec664b49655fc9692165fb");
	vector<uint8_t> bytes = {0xe3, 0x53, 0x66, 0x76, 0x19, 0xec, 0x66, 0x4b, 0x49, 0x65, 0x5F, 0xc9, 0x69, 0x21, 0x65, 0xfb};
	EXPECT_EQ(xxh128.digestValue(), bytes);
}
#endif


TEST(HashFunctionTest, createMD5) {
	auto hashfunc = HashFunction::create("md5");
	ASSERT_TRUE(dynamic_cast<MD5HashFunction*>(hashfunc.get()) != nullptr);
	hashfunc->update("0123456789");
	EXPECT_EQ(hashfunc->digestString(), "781e5e245d69b566979b86e28d23f2c7");

	hashfunc = HashFunction::create("md5", "0123456789");
	ASSERT_TRUE(dynamic_cast<MD5HashFunction*>(hashfunc.get()) != nullptr);
	EXPECT_EQ(hashfunc->digestString(), "781e5e245d69b566979b86e28d23f2c7");
}


TEST(HashFunctionTest, createXXH32) {
	auto hashfunc = HashFunction::create("xxh32");
	ASSERT_TRUE(dynamic_cast<XXH32HashFunction*>(hashfunc.get()) != nullptr);
	hashfunc->update("0123456789");
	EXPECT_EQ(hashfunc->digestString(), "950c9c0a");

	hashfunc = HashFunction::create("xxh32", "0123456789");
	ASSERT_TRUE(dynamic_cast<XXH32HashFunction*>(hashfunc.get()) != nullptr);
	EXPECT_EQ(hashfunc->digestString(), "950c9c0a");
}


TEST(HashFunctionTest, createXXH64) {
	auto hashfunc = HashFunction::create("xxh64");
	ASSERT_TRUE(dynamic_cast<XXH64HashFunction*>(hashfunc.get()) != nullptr);
	hashfunc->update("0123456789");
	EXPECT_EQ(hashfunc->digestString(), "3f5fc178a81867e7");

	hashfunc = HashFunction::create("xxh64", "0123456789");
	ASSERT_TRUE(dynamic_cast<XXH64HashFunction*>(hashfunc.get()) != nullptr);
	EXPECT_EQ(hashfunc->digestString(), "3f5fc178a81867e7");
}


#ifdef ENABLE_XXH128
TEST(HashFunctionTest, createXXH128) {
	auto hashfunc = HashFunction::create("xxh128");
	ASSERT_TRUE(dynamic_cast<XXH128HashFunction*>(hashfunc.get()) != nullptr);
	hashfunc->update("0123456789");
	EXPECT_EQ(hashfunc->digestString(), "e353667619ec664b49655fc9692165fb");

	hashfunc = HashFunction::create("xxh128", "0123456789");
	ASSERT_TRUE(dynamic_cast<XXH128HashFunction*>(hashfunc.get()) != nullptr);
	EXPECT_EQ(hashfunc->digestString(), "e353667619ec664b49655fc9692165fb");
}
#endif


TEST(HashFunctionTest, createFail) {
	ASSERT_TRUE(HashFunction::create("not-available") == nullptr);
}
