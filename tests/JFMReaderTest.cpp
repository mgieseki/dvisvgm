/*************************************************************************
** JFMReaderTest.cpp                                                    **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <string>
#include <vector>
#include "JFM.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


class JFMReaderTest : public ::testing::Test
{
	public:
		JFMReaderTest () : jfm(0) {}

		void SetUp () override {
			string fname = string(SRCDIR)+"/data/cidjmgr0-h.tfm";
			ifstream ifs(fname.c_str(), ios::binary);
			ASSERT_TRUE(ifs.is_open()) << "failed opening " << fname;
			jfm = new JFM(ifs);
		}

		void TearDown () override {
			delete jfm;
		}

	protected:
		JFM *jfm;
};


TEST_F(JFMReaderTest, properties) {
	ASSERT_EQ(jfm->getChecksum(), 0);
	ASSERT_FALSE(jfm->verticalLayout());
	ASSERT_EQ(jfm->firstChar(), 0);
	ASSERT_EQ(jfm->lastChar(), 1);
	ASSERT_EQ(jfm->minChar(), 13143);
	ASSERT_EQ(jfm->maxChar(), 14696);
	ASSERT_DOUBLE_EQ(jfm->getDesignSize(), 10.0*72.0/72.27);
}


TEST_F(JFMReaderTest, charWidth) {
	uint16_t minchar = jfm->minChar();
	uint16_t maxchar = jfm->maxChar();
	ASSERT_NEAR(jfm->getCharWidth(0), 9.963, 0.001);
	ASSERT_NEAR(jfm->getCharWidth(minchar), 4.981, 0.001);
	ASSERT_NEAR(jfm->getCharWidth(minchar+1), 4.981, 0.001);
	ASSERT_NEAR(jfm->getCharWidth(maxchar), 4.981, 0.001);
	ASSERT_NEAR(jfm->getCharWidth(maxchar+1), 9.963, 0.001);
}


TEST_F(JFMReaderTest, charHeight) {
	uint16_t minchar = jfm->minChar();
	uint16_t maxchar = jfm->maxChar();
	ASSERT_NEAR(jfm->getCharHeight(0), 8.767, 0.001);
	ASSERT_NEAR(jfm->getCharHeight(minchar), 8.767, 0.001);
	ASSERT_NEAR(jfm->getCharHeight(minchar+1), 8.767, 0.001);
	ASSERT_NEAR(jfm->getCharHeight(maxchar), 8.767, 0.001);
	ASSERT_NEAR(jfm->getCharHeight(maxchar+1), 8.767, 0.001);
}


TEST_F(JFMReaderTest, charDepth) {
	uint16_t minchar = jfm->minChar();
	uint16_t maxchar = jfm->maxChar();
	ASSERT_NEAR(jfm->getCharDepth(0), 1.195, 0.001);
	ASSERT_NEAR(jfm->getCharDepth(minchar), 1.195, 0.001);
	ASSERT_NEAR(jfm->getCharDepth(minchar+1), 1.195, 0.001);
	ASSERT_NEAR(jfm->getCharDepth(maxchar), 1.195, 0.001);
	ASSERT_NEAR(jfm->getCharDepth(maxchar+1), 1.195, 0.001);
}


TEST_F(JFMReaderTest, italicCorr) {
	uint16_t minchar = jfm->minChar();
	uint16_t maxchar = jfm->maxChar();
	ASSERT_DOUBLE_EQ(jfm->getItalicCorr(0), 0);
	ASSERT_DOUBLE_EQ(jfm->getItalicCorr(minchar), 0);
	ASSERT_DOUBLE_EQ(jfm->getItalicCorr(minchar+1), 0);
	ASSERT_DOUBLE_EQ(jfm->getItalicCorr(maxchar), 0);
	ASSERT_DOUBLE_EQ(jfm->getItalicCorr(maxchar+1), 0);
}


TEST_F(JFMReaderTest, params) {
	ASSERT_DOUBLE_EQ(jfm->getSpace(), 0);
	ASSERT_DOUBLE_EQ(jfm->getSpaceShrink(), 0.);
	ASSERT_NEAR(jfm->getSpaceStretch(), 0.996, 0.001);
	ASSERT_NEAR(jfm->getQuad(), 9.963, 0.001);
}
