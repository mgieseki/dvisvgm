/*************************************************************************
** OFMReaderTest.cpp                                                    **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2024 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <string>
#include <vector>
#include "Length.hpp"
#include "OFM.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


class OFMReaderTest : public ::testing::Test {
	public:
		void SetUp () override {
			string fname = string(SRCDIR)+"/data/omarab.ofm";
			ifstream ifs(fname, ios::binary);
			ASSERT_TRUE(ifs.is_open()) << "failed to open " << fname;
			ofm0.read(ifs);
			scale0 = ofm0.getDesignSize();
			ifs.close();

			fname = string(SRCDIR)+"/data/upjf-g.ofm";
			ifs.open(fname, ios::binary);
			ASSERT_TRUE(ifs.is_open()) << "failed to open " << fname;
			ofm1.read(ifs);
			scale1 = ofm1.getDesignSize();
		}

	protected:
		OFM0 ofm0;
		OFM1 ofm1;
		double scale0=0, scale1=0;
};


TEST_F(OFMReaderTest, properties) {
	EXPECT_EQ(ofm0.getChecksum(), 0x9DCC4880u);
	EXPECT_FALSE(ofm0.verticalLayout());
	EXPECT_EQ(ofm0.firstChar(), 0x21u);
	EXPECT_EQ(ofm0.lastChar(), 0x059Fu);
	EXPECT_NEAR(ofm0.getDesignSize(), 10.0*Length::pt2bp, 0.000001);

	EXPECT_EQ(ofm1.getChecksum(), 0u);
	EXPECT_FALSE(ofm1.verticalLayout());
	EXPECT_EQ(ofm1.firstChar(), 0x2600u);
	EXPECT_EQ(ofm1.lastChar(), 0xFFFFu);
	EXPECT_NEAR(ofm1.getDesignSize(), 10.0*Length::pt2bp, 0.000001);
}


TEST_F(OFMReaderTest, charWidth) {
	EXPECT_NEAR(ofm0.getCharWidth(0x029D), 0.853*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getCharWidth(0x02A0), 0.444*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getCharWidth(0x1000), 0, 0.0001);

	EXPECT_NEAR(ofm1.getCharWidth(0x2600), 1.0*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharWidth(0xFF00), 0.5*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharWidth(0xFF01), 0.5*scale1, 0.0001);
}


TEST_F(OFMReaderTest, charHeight) {
	EXPECT_NEAR(ofm0.getCharHeight(0x029D), 0.298*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getCharHeight(0x02A0), 0.685*scale0, 0.0001);
	EXPECT_DOUBLE_EQ(ofm0.getCharHeight(0x1000), 0);

	EXPECT_NEAR(ofm1.getCharHeight(0x2600), 0.9*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharHeight(0xFF00), 0.9*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharHeight(0xFF01), 0.9*scale1, 0.0001);
}


TEST_F(OFMReaderTest, charDepth) {
	EXPECT_NEAR(ofm0.getCharDepth(0x029D), 0.3*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getCharDepth(0x02A0), 0, 0.0001);
	EXPECT_NEAR(ofm0.getCharDepth(0x1000), 0, 0.0001);

	EXPECT_NEAR(ofm1.getCharDepth(0x2600), 0.1*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharDepth(0xFF00), 0.1*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getCharDepth(0xFF01), 0.1*scale1, 0.0001);
}


TEST_F(OFMReaderTest, italicCorr) {
	EXPECT_DOUBLE_EQ(ofm0.getItalicCorr(0x029D), 0);
	EXPECT_DOUBLE_EQ(ofm0.getItalicCorr(0x02A0), 0);
	EXPECT_DOUBLE_EQ(ofm0.getItalicCorr(0x1000), 0);

	EXPECT_DOUBLE_EQ(ofm1.getItalicCorr(0x2600), 0);
	EXPECT_DOUBLE_EQ(ofm1.getItalicCorr(0xFF00), 0);
	EXPECT_DOUBLE_EQ(ofm1.getItalicCorr(0xFF01), 0);
}


TEST_F(OFMReaderTest, params) {
	EXPECT_NEAR(ofm0.getSpace(), 0.5*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getSpaceShrink(), 0.1*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getSpaceStretch(), 0.3*scale0, 0.0001);
	EXPECT_NEAR(ofm0.getQuad(), 1.0*scale0, 0.0001);

	EXPECT_NEAR(ofm1.getSpace(), 0, 0.0001);
	EXPECT_NEAR(ofm1.getSpaceShrink(), 0, 0.0001);
	EXPECT_NEAR(ofm1.getSpaceStretch(), 0.1*scale1, 0.0001);
	EXPECT_NEAR(ofm1.getQuad(), 1.0*scale1, 0.0001);
}
