/*************************************************************************
** JFMReaderTest.cpp                                                    **
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
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "JFM.hpp"
#include "utility.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


class JFMReaderTest : public ::testing::Test {
	public:
		void SetUp () override {
			string fname = string(SRCDIR)+"/data/cidjmgr0-h.tfm";
			ifstream ifs(fname, ios::binary);
			ASSERT_TRUE(ifs.is_open()) << "failed opening " << fname;
			jfm = util::make_unique<JFM>(ifs);
		}

	protected:
		unique_ptr<JFM> jfm;
};


TEST_F(JFMReaderTest, properties) {
	EXPECT_EQ(jfm->getChecksum(), 0u);
	EXPECT_FALSE(jfm->verticalLayout());
	EXPECT_EQ(jfm->firstChar(), 0);
	EXPECT_EQ(jfm->lastChar(), 1);
	EXPECT_EQ(jfm->minChar(), 13143u);
	EXPECT_EQ(jfm->maxChar(), 14696u);
	EXPECT_DOUBLE_EQ(jfm->getDesignSize(), 10.0*72.0/72.27);
}


TEST_F(JFMReaderTest, charWidth) {
	uint32_t minchar = jfm->minChar();
	uint32_t maxchar = jfm->maxChar();
	EXPECT_NEAR(jfm->getCharWidth(0), 9.963, 0.001);
	EXPECT_NEAR(jfm->getCharWidth(minchar), 4.981, 0.001);
	EXPECT_NEAR(jfm->getCharWidth(minchar+1), 4.981, 0.001);
	EXPECT_NEAR(jfm->getCharWidth(maxchar), 4.981, 0.001);
	EXPECT_NEAR(jfm->getCharWidth(maxchar+1), 9.963, 0.001);
}


TEST_F(JFMReaderTest, charHeight) {
	uint32_t minchar = jfm->minChar();
	uint32_t maxchar = jfm->maxChar();
	EXPECT_NEAR(jfm->getCharHeight(0), 8.767, 0.001);
	EXPECT_NEAR(jfm->getCharHeight(minchar), 8.767, 0.001);
	EXPECT_NEAR(jfm->getCharHeight(minchar+1), 8.767, 0.001);
	EXPECT_NEAR(jfm->getCharHeight(maxchar), 8.767, 0.001);
	EXPECT_NEAR(jfm->getCharHeight(maxchar+1), 8.767, 0.001);
}


TEST_F(JFMReaderTest, charDepth) {
	uint32_t minchar = jfm->minChar();
	uint32_t maxchar = jfm->maxChar();
	EXPECT_NEAR(jfm->getCharDepth(0), 1.195, 0.001);
	EXPECT_NEAR(jfm->getCharDepth(minchar), 1.195, 0.001);
	EXPECT_NEAR(jfm->getCharDepth(minchar+1), 1.195, 0.001);
	EXPECT_NEAR(jfm->getCharDepth(maxchar), 1.195, 0.001);
	EXPECT_NEAR(jfm->getCharDepth(maxchar+1), 1.195, 0.001);
}


TEST_F(JFMReaderTest, italicCorr) {
	uint32_t minchar = jfm->minChar();
	uint32_t maxchar = jfm->maxChar();
	EXPECT_DOUBLE_EQ(jfm->getItalicCorr(0), 0);
	EXPECT_DOUBLE_EQ(jfm->getItalicCorr(minchar), 0);
	EXPECT_DOUBLE_EQ(jfm->getItalicCorr(minchar+1), 0);
	EXPECT_DOUBLE_EQ(jfm->getItalicCorr(maxchar), 0);
	EXPECT_DOUBLE_EQ(jfm->getItalicCorr(maxchar+1), 0);
}


TEST_F(JFMReaderTest, params) {
	EXPECT_DOUBLE_EQ(jfm->getSpace(), 0);
	EXPECT_DOUBLE_EQ(jfm->getSpaceShrink(), 0.);
	EXPECT_NEAR(jfm->getSpaceStretch(), 0.996, 0.001);
	EXPECT_NEAR(jfm->getQuad(), 9.963, 0.001);
}
