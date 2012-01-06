/*************************************************************************
** FontMapTest.cpp                                                      **
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
#include <fstream>
#include "FontMap.h"

using std::ifstream;

TEST(FontMapTest, pdf_map) {
	const char *fname = "tests/dvipdfm_test.map";
	ifstream ifs(fname);
	if (ifs) {
		FontMap fm(fname);
		EXPECT_EQ(strcmp(fm.lookup("MyriadPro-Bold-8t"), "MyriadPro-Bold"), 0);
		EXPECT_EQ(strcmp(fm.lookup("cmbsy8"), "cmbsy7"), 0);
		// entry without mapped name
		EXPECT_EQ(fm.lookup("msam10"), (const char*)0);
		EXPECT_EQ(fm.lookup("msbm10"), (const char*)0);
		EXPECT_EQ(fm.lookup("msbm10"), (const char*)0);
		EXPECT_EQ(fm.lookup("i don't exist"), (const char*)0);

		EXPECT_EQ(strcmp(fm.encoding("MyriadPro-Bold-8t"), "my-lf-t1"), 0);
		EXPECT_EQ(fm.encoding("cmbsy8"), (const char*)0);
	}
}


TEST(FontMapTest, ps_map) {
	const char *fname = "tests/dvips_test.map";
	ifstream ifs(fname);
	if (ifs) {
		FontMap fm(fname);
		EXPECT_EQ(strcmp(fm.lookup("MyriadPro-Bold-8t"), "MyriadPro-Bold"), 0);
		EXPECT_EQ(strcmp(fm.lookup("cmbsy8"), "cmbsy7"), 0);
		// entry without mapped name
		EXPECT_EQ(fm.lookup("msam10"), (const char*)0);
		EXPECT_EQ(fm.lookup("msbm10"), (const char*)0);
		EXPECT_EQ(fm.lookup("msbm10"), (const char*)0);
		EXPECT_EQ(fm.lookup("i don't exist"), (const char*)0);

		EXPECT_EQ(strcmp(fm.encoding("MyriadPro-Bold-8t"), "my-lf-t1"), 0);
		EXPECT_EQ(fm.encoding("cmbsy8"), (const char*)0);

	}
}
