/*************************************************************************
** FontManagerTest.cpp                                                  **
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
#include "Font.hpp"
#include "FontManager.hpp"

class FontManagerTest : public ::testing::Test {
	public:
		FontManagerTest () : fm(FontManager::instance()) {
			fm.registerFont(10, "cmr10", 1274110073, 10, 10);
			fm.registerFont(11, "cmr10", 1274110073, 10, 12);
			fm.registerFont( 9, "cmr10", 1274110073, 10, 14);
		}

	protected:
		FontManager &fm;
};


TEST_F(FontManagerTest, fontID1) {
	EXPECT_EQ(fm.fontID(10), 0);
	EXPECT_EQ(fm.fontID(11), 1);
	EXPECT_EQ(fm.fontID(9), 2);
	EXPECT_EQ(fm.fontID(1), -1);
}


TEST_F(FontManagerTest, font_ID2) {
	EXPECT_EQ(fm.fontID("cmr10"), 0);
}


TEST_F(FontManagerTest, getFont) {
	const Font *f1 = fm.getFont(10);
	EXPECT_TRUE(f1);
	EXPECT_EQ(f1->name(), "cmr10");
	EXPECT_TRUE(dynamic_cast<const PhysicalFontImpl*>(f1));

	const Font *f2 = fm.getFont(11);
	EXPECT_TRUE(f2);
	EXPECT_NE(f1, f2);
	EXPECT_EQ(f2->name(), "cmr10");
	EXPECT_TRUE(dynamic_cast<const PhysicalFontProxy*>(f2));
	EXPECT_EQ(f2->uniqueFont(), f1);
}


TEST_F(FontManagerTest, getFontById) {
	EXPECT_EQ(fm.getFont(10), fm.getFontById(0));
	EXPECT_EQ(fm.getFont("cmr10"), fm.getFontById(0));
}

