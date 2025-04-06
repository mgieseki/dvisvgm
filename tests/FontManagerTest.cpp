/*************************************************************************
** FontManagerTest.cpp                                                  **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef SRCDIR
#define SRCDIR "."
#endif


class FontManagerTest : public ::testing::Test {
	public:
		FontManagerTest () : fm(FontManager::instance()) {
			fm.registerFont(10, "cmr10", 1274110073, 10, 10);
			fm.registerFont(11, "cmr10", 1274110073, 10, 12);
			fm.registerFont( 9, "cmr10", 1274110073, 10, 14);
			fm.registerFont(12, SRCDIR "/data/lmmono12-regular.otf", 0, 12, _fontStyle, Color(.0, .0, 1.0));
		}

	protected:
		FontManager &fm;
		FontStyle _fontStyle;
};


TEST_F(FontManagerTest, fontID1) {
	EXPECT_EQ(fm.fontID(10), 0);
	EXPECT_EQ(fm.fontID(11), 1);
	EXPECT_EQ(fm.fontID(9), 2);
	EXPECT_EQ(fm.fontID(12), 3);
	EXPECT_EQ(fm.fontID(1), -1);
}


TEST_F(FontManagerTest, fontID2) {
	EXPECT_EQ(fm.fontID("cmr10"), 0);
	EXPECT_EQ(fm.fontID("nf0"), 3);
}


TEST_F(FontManagerTest, fontID3) {
	EXPECT_EQ(fm.fontID("nf0", 12), 3);
}


TEST_F(FontManagerTest, fontID4) {
	EXPECT_EQ(fm.fontID(fm.getFontById(0)), 0);
	EXPECT_EQ(fm.fontID(fm.getFontById(1)), 1);
	EXPECT_EQ(fm.fontID(fm.getFontById(2)), 2);
	EXPECT_EQ(fm.fontID(fm.getFontById(3)), 3);
}


TEST_F(FontManagerTest, fontnum) {
	EXPECT_EQ(fm.fontnum(0), 10);
	EXPECT_EQ(fm.fontnum(1), 11);
	EXPECT_EQ(fm.fontnum(2),  9);
	EXPECT_EQ(fm.fontnum(3), 12);
}


TEST_F(FontManagerTest, getFont) {
	const Font *f1 = fm.getFont(10);
	EXPECT_TRUE(f1);
	EXPECT_EQ(f1->name(), "cmr10");
	EXPECT_TRUE(dynamic_cast<const PhysicalFontImpl*>(f1));
	EXPECT_EQ(f1->color(), Color::BLACK);

	const Font *f2 = fm.getFont(11);
	EXPECT_TRUE(f2);
	EXPECT_NE(f1, f2);
	EXPECT_EQ(f2->name(), "cmr10");
	EXPECT_TRUE(dynamic_cast<const PhysicalFontRef*>(f2));
	EXPECT_EQ(f2->uniqueFont(), f1);
	EXPECT_EQ(f2->color(), Color::BLACK);

	const Font *f3 = fm.getFont(12);
	EXPECT_TRUE(f3);
	EXPECT_NE(f2, f3);
	EXPECT_EQ(f3->name(), "nf0");
	EXPECT_TRUE(dynamic_cast<const NativeFontImpl*>(f3));
	EXPECT_TRUE(dynamic_cast<const PhysicalFont*>(f3));
	EXPECT_EQ(f3->uniqueFont(), f3);
	EXPECT_EQ(f3->color(), Color(.0, .0, 1.0));
}


TEST_F(FontManagerTest, font_cast) {
	const Font *f1 = fm.getFont(10);
	EXPECT_TRUE(f1);
	EXPECT_EQ(font_cast<const PhysicalFont*>(f1), f1);
	EXPECT_EQ(font_cast<const NativeFont*>(f1), nullptr);
	EXPECT_EQ(font_cast<const VirtualFont*>(f1), nullptr);

	const Font *f2 = fm.getFont(11);
	EXPECT_TRUE(f2);
	EXPECT_EQ(font_cast<const PhysicalFont*>(f2), f2);
	EXPECT_EQ(font_cast<const NativeFont*>(f1), nullptr);
	EXPECT_EQ(font_cast<const VirtualFont*>(f1), nullptr);

	const Font *f3 = fm.getFont(12);
	EXPECT_TRUE(f3);
	EXPECT_EQ(font_cast<const PhysicalFont*>(f3), f3);
	EXPECT_EQ(font_cast<const NativeFont*>(f3), f3);
	EXPECT_EQ(font_cast<const VirtualFont*>(f3), nullptr);
}


TEST_F(FontManagerTest, getFontById) {
	EXPECT_EQ(fm.getFont(10), fm.getFontById(0));
	EXPECT_EQ(fm.getFont("cmr10"), fm.getFontById(0));
	EXPECT_EQ(fm.getFont(12), fm.getFontById(3));
}
