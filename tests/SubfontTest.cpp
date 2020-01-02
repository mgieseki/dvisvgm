/*************************************************************************
** SubfontTest.cpp                                                      **
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
#include "Subfont.hpp"
#include "testutil.hpp"

using namespace std;

TEST(SubfontTest, collect_subfonts) {
	try {
		if (SubfontDefinition *sfd = SubfontDefinition::lookup("sample")) {
			vector<Subfont*> subfonts;
			const char *ids_cmp[] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "x1"};
			int size = sizeof(ids_cmp)/sizeof(Subfont*);
			EXPECT_EQ(sfd->subfonts(subfonts), size);
			for (int i=0; i < size; i++) {
				EXPECT_EQ(subfonts[i]->id(), ids_cmp[i]);
			}
		}
	}
	catch (SubfontException &e) {
		FAIL() << e.what();
	}
}


TEST(SubfontTest, read_table) {
	try {
		if (SubfontDefinition *sfd = SubfontDefinition::lookup("sampl")) {
			// check scanning of single value entries
			Subfont *subfont = sfd->subfont("02");
			ASSERT_NE(subfont, nullptr);
			EXPECT_EQ(subfont->id(), "02");
			EXPECT_EQ(subfont->decode(0), 0xff45);
			EXPECT_EQ(subfont->decode(1), 0xff46);
			EXPECT_EQ(subfont->decode(2), 0xff47);
			EXPECT_EQ(subfont->decode(8), 0xff4d);
			EXPECT_EQ(subfont->decode(32), 0x3047);
			EXPECT_EQ(subfont->decode(255), 0x03ba);

			// check scanning of ranges
			subfont = sfd->subfont("x1");
			ASSERT_NE(subfont, nullptr);
			EXPECT_EQ(subfont->id(), "x1");
			EXPECT_EQ(subfont->decode(0), 0x0010);
			EXPECT_EQ(subfont->decode(1), 0x0011);
			EXPECT_EQ(subfont->decode(2), 0x0012);
			EXPECT_EQ(subfont->decode(0x20), 0x0030);
			EXPECT_EQ(subfont->decode(0x21), 0x1010);
			EXPECT_EQ(subfont->decode(0x41), 0x1030);
			EXPECT_EQ(subfont->decode(0x42), 0xe000);
			EXPECT_EQ(subfont->decode(0x43), 0);

			//check scanning of offset values
			EXPECT_EQ(subfont->decode(0x9f), 0);
			EXPECT_EQ(subfont->decode(0xa0), 0x1000);
			EXPECT_EQ(subfont->decode(0xa1), 0x2000);
			EXPECT_EQ(subfont->decode(0xa2), 0);
		}
		else
			WARNING("sample.sfd not found");
	}
	catch (SubfontException &e) {
		FAIL() << e.what() << " in line " << e.lineno();
	}
}
