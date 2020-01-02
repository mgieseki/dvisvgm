/*************************************************************************
** MapLineTest.cpp                                                      **
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
#include <sstream>
#include "MapLine.hpp"
#include "Subfont.hpp"
#include "testutil.hpp"

using namespace std;

TEST(MapLineTest, psline1) {
	istringstream iss("texname0 TEXNAME0 <texname.pfb <encname.enc");
	MapLine mapline(iss);
	EXPECT_EQ(mapline.texname(), "texname0");
	EXPECT_EQ(mapline.psname(), "TEXNAME0");
	EXPECT_EQ(mapline.fontfname(), "texname.pfb");
	EXPECT_EQ(mapline.encname(), "encname");
	EXPECT_DOUBLE_EQ(mapline.slant(), 0);
	EXPECT_DOUBLE_EQ(mapline.extend(), 1);
	EXPECT_DOUBLE_EQ(mapline.bold(), 0);
}

TEST(MapLineTest, psline2) {
	istringstream iss("texname0 TEXNAME0 <<texname.pfb <encname.enc");
	MapLine mapline(iss);
	EXPECT_EQ(mapline.texname(), "texname0");
	EXPECT_EQ(mapline.psname(), "TEXNAME0");
	EXPECT_EQ(mapline.fontfname(), "texname.pfb");
	EXPECT_EQ(mapline.encname(), "encname");
	EXPECT_DOUBLE_EQ(mapline.slant(), 0);
	EXPECT_DOUBLE_EQ(mapline.extend(), 1);
	EXPECT_DOUBLE_EQ(mapline.bold(), 0);
}

TEST(MapLineTest, psline3) {
	istringstream iss("texname0 TEXNAME0 \".123 SlantFont .456 ExtendFont\" <encname.enc <[texname.ttf");
	MapLine mapline(iss);
	EXPECT_EQ(mapline.texname(), "texname0");
	EXPECT_EQ(mapline.psname(), "TEXNAME0");
	EXPECT_EQ(mapline.fontfname(), "texname.ttf");
	EXPECT_EQ(mapline.encname(), "encname");
	EXPECT_DOUBLE_EQ(mapline.slant(), 0.123);
	EXPECT_DOUBLE_EQ(mapline.extend(), 0.456);
	EXPECT_DOUBLE_EQ(mapline.bold(), 0);
}

TEST(MapLineTest, psline4) {
	istringstream iss("texname0 TEXNAME0 <encname.enc \".123 SlantFont IgnoreMe .456 ExtendFont\" <texname.ttf");
	MapLine mapline(iss);
	EXPECT_EQ(mapline.texname(), "texname0");
	EXPECT_EQ(mapline.psname(), "TEXNAME0");
	EXPECT_EQ(mapline.fontfname(), "texname.ttf");
	EXPECT_EQ(mapline.encname(), "encname");
	EXPECT_DOUBLE_EQ(mapline.slant(), 0.123);
	EXPECT_DOUBLE_EQ(mapline.extend(), 0.456);
	EXPECT_DOUBLE_EQ(mapline.bold(), 0);
}

TEST(MapLineTest, pdfline1) {
	istringstream iss("texname");
	MapLine mapline(iss);
   EXPECT_EQ(mapline.texname(), "texname");
   EXPECT_EQ(mapline.psname(), "");
   EXPECT_EQ(mapline.encname(), "");
   EXPECT_EQ(mapline.fontfname(), "");
}

TEST(MapLineTest, pdfline2) {
	istringstream iss("gbk unicode simsun.ttc");
	MapLine mapline(iss);
   EXPECT_EQ(mapline.texname(), "gbk");
   EXPECT_EQ(mapline.psname(), "");
   EXPECT_EQ(mapline.encname(), "unicode");
   EXPECT_EQ(mapline.fontfname(), "simsun.ttc");
	EXPECT_EQ(mapline.fontindex(), 0);
	EXPECT_TRUE(mapline.sfd() == 0);
   EXPECT_EQ(mapline.fontindex(), 0);
   EXPECT_DOUBLE_EQ(mapline.slant(), 0);
   EXPECT_DOUBLE_EQ(mapline.extend(), 1);
}

TEST(MapLineTest, pdfline3) {
	istringstream iss("gbk@UGBK@10 unicode simsun.ttc -v 50 -r -s .123 -b 1 -e 0.456");
	MapLine mapline(iss);
   EXPECT_EQ(mapline.texname(), "gbk10");
   EXPECT_EQ(mapline.psname(), "");
   EXPECT_EQ(mapline.encname(), "unicode");
   EXPECT_EQ(mapline.fontfname(), "simsun.ttc");
	EXPECT_EQ(mapline.fontindex(), 0);
	EXPECT_DOUBLE_EQ(mapline.slant(), 0.123);
	EXPECT_DOUBLE_EQ(mapline.extend(), 0.456);
	EXPECT_DOUBLE_EQ(mapline.bold(), 1);
	if (mapline.sfd() != 0)  // if UGBK.sfd is installed, check SFD name
		EXPECT_EQ(mapline.sfd()->name(), "UGBK");
	else
		WARNING("UGBK.sfd not found");
}

TEST(MapLineTest, pdfline4) {
	istringstream iss("gbk@UGBK@ default :1:!simsun.ttc/UCS,Bold -e.345");
	MapLine mapline(iss);
   EXPECT_EQ(mapline.texname(), "gbk");
   EXPECT_EQ(mapline.psname(), "");
   EXPECT_EQ(mapline.encname(), "");  // encodings "default" and "none" are replaced with ""
	EXPECT_EQ(mapline.fontfname(), "simsun.ttc");
	EXPECT_EQ(mapline.fontindex(), 1);
	EXPECT_DOUBLE_EQ(mapline.slant(), 0);
	EXPECT_DOUBLE_EQ(mapline.extend(), 0.345);
	if (mapline.sfd() != 0)  // if UGBK.sfd is installed, check SFD name
		EXPECT_EQ(mapline.sfd()->name(), "UGBK");
	else
		WARNING("UGBK.sfd not found");
}


TEST(MapLineTest, fail) {
	EXPECT_THROW(MapLine("texname -"), MapLineException);
	EXPECT_THROW(MapLine("texname -s"), MapLineException);
	EXPECT_THROW(MapLine("texname -e"), MapLineException);
	EXPECT_THROW(MapLine("texname -b"), MapLineException);
	EXPECT_THROW(MapLine("texname -i"), MapLineException);
	EXPECT_THROW(MapLine("texname -p"), MapLineException);
	EXPECT_THROW(MapLine("texname -v"), MapLineException);
	EXPECT_THROW(MapLine("texname -w"), MapLineException);
	EXPECT_THROW(MapLine("texname -Z"), MapLineException);
}
