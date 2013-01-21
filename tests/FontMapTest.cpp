/*************************************************************************
** FontMapTest.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "FontMap.h"

using std::ifstream;

TEST(FontMapTest, pdf_map) {
	const char *fname = "tests/dvipdfm_test.map";
	ifstream ifs(fname);
	if (ifs) {
		FontMap &fm = FontMap::instance();
		fm.read(fname);
		const FontMap::Entry *entry;
		ASSERT_TRUE((entry = fm.lookup("MyriadPro-Bold-8t")) != 0);
		EXPECT_EQ(entry->fontname, "MyriadPro-Bold");
		EXPECT_EQ(entry->encname, "my-lf-t1");
		ASSERT_TRUE((entry = fm.lookup("cmbsy8")) != 0);
		EXPECT_EQ(entry->fontname, "cmbsy7");
		EXPECT_EQ(entry->encname, "");

		// entry without mapped name
		EXPECT_TRUE(fm.lookup("msam10") == 0);
		EXPECT_TRUE(fm.lookup("msbm10") == 0);
		EXPECT_TRUE(fm.lookup("msbm10") == 0);
		EXPECT_TRUE(fm.lookup("i don't exist") ==  0);
	}
}


TEST(FontMapTest, ps_map) {
	const char *fname = "tests/dvips_test.map";
	ifstream ifs(fname);
	if (ifs) {
		FontMap &fm = FontMap::instance();
		fm.read(fname);
		const FontMap::Entry *entry;
		ASSERT_TRUE((entry = fm.lookup("MyriadPro-Bold-8t")) != 0);
		EXPECT_EQ(entry->fontname, "MyriadPro-Bold");
		EXPECT_EQ(entry->encname, "my-lf-t1");
		ASSERT_TRUE((entry = fm.lookup("cmbsy8")) != 0);
		EXPECT_EQ(entry->fontname, "cmbsy7");
		EXPECT_EQ(entry->encname, "");

		// entry without mapped name
		EXPECT_TRUE(fm.lookup("msam10") == 0);
		EXPECT_TRUE(fm.lookup("msbm10") == 0);
		EXPECT_TRUE(fm.lookup("msbm10") == 0);
		EXPECT_TRUE(fm.lookup("i don't exist") ==  0);
	}
}
