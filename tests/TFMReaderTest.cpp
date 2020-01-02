/*************************************************************************
** TFMReaderTest.cpp                                                    **
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
#include <cstdlib>
#include <memory>
#include <fstream>
#include <string>
#include <vector>
#include "TFM.hpp"
#include "utility.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


class TFMReaderTest : public ::testing::Test {
	public:
		void SetUp () override {
			string fname = string(SRCDIR)+"/data/cmr10.tfm";
			ifstream ifs(fname, ios::binary);
			ASSERT_TRUE(ifs.is_open()) << "failed opening " << fname;
			tfm = util::make_unique<TFM>(ifs);
		}

	protected:
		unique_ptr<TFM> tfm;
};


TEST_F(TFMReaderTest, properties) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_EQ(tfm->getChecksum(), 0x4BF16079u);
	EXPECT_FALSE(tfm->verticalLayout());
	EXPECT_EQ(tfm->firstChar(), 0);
	EXPECT_EQ(tfm->lastChar(), 127);
	EXPECT_DOUBLE_EQ(tfm->getDesignSize(), 10.0*72.0/72.27);
}


TEST_F(TFMReaderTest, charWidth) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_NEAR(tfm->getCharWidth('M'), 9.132, 0.001);
	EXPECT_NEAR(tfm->getCharWidth('g'), 4.981, 0.001);
	EXPECT_DOUBLE_EQ(tfm->getCharWidth(200), 0);
}


TEST_F(TFMReaderTest, charHeight) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_NEAR(tfm->getCharHeight('M'), 6.808, 0.001);
	EXPECT_NEAR(tfm->getCharHeight('g'), 4.289, 0.001);
	EXPECT_DOUBLE_EQ(tfm->getCharHeight(200), 0);
}


TEST_F(TFMReaderTest, charDepth) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_DOUBLE_EQ(tfm->getCharDepth('M'), 0);
	EXPECT_NEAR(tfm->getCharDepth('g'), 1.937, 0.001);
	EXPECT_DOUBLE_EQ(tfm->getCharDepth(200), 0);
}


TEST_F(TFMReaderTest, italicCorr) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_DOUBLE_EQ(tfm->getItalicCorr('M'), 0);
	EXPECT_NEAR(tfm->getItalicCorr('g'), 0.138, 0.001);
	EXPECT_DOUBLE_EQ(tfm->getItalicCorr(200), 0);
}


TEST_F(TFMReaderTest, params) {
	ASSERT_NE(tfm, nullptr);
	EXPECT_NEAR(tfm->getSpace(), 3.321, 0.001);
	EXPECT_NEAR(tfm->getSpaceShrink(), 1.107, 0.001);
	EXPECT_NEAR(tfm->getSpaceStretch(), 1.66, 0.001);
	EXPECT_NEAR(tfm->getQuad(), 9.963, 0.001);
}
