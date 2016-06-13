/*************************************************************************
** LengthTest.cpp                                                       **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <string>
#include "Length.h"

using namespace std;

TEST(LengthTest, set1) {
	Length len;
	len.set(1, Length::PT);
	EXPECT_DOUBLE_EQ(len.pt(), 1);

	len.set(72, Length::BP);
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.bp(), 72);
	EXPECT_DOUBLE_EQ(len.in(), 1);

	len.set(1, Length::IN);
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);

	len.set(1, Length::PC);
	EXPECT_DOUBLE_EQ(len.pt(), 12);

	len.set(2.54, Length::CM);
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.cm(), 2.54);
	EXPECT_DOUBLE_EQ(len.mm(), 25.4);

	len.set(25.4, Length::MM);
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.cm(), 2.54);
	EXPECT_DOUBLE_EQ(len.mm(), 25.4);

	len.set(1, Length::DD);
	EXPECT_DOUBLE_EQ(len.pt(), 1238.0/1157);

	len.set(1, Length::CC);
	EXPECT_DOUBLE_EQ(len.dd(), 12.0);

	len.set(1, Length::SP);
	EXPECT_DOUBLE_EQ(len.pt(), 1.0/65536);
}


TEST(LengthTest, set2) {
	Length len;
	len.set(1, "pt");
	EXPECT_DOUBLE_EQ(len.pt(), 1);

	len.set(72, "bp");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.bp(), 72);
	EXPECT_DOUBLE_EQ(len.in(), 1);

	len.set(1, "in");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);

	len.set(1, "pc");
	EXPECT_DOUBLE_EQ(len.pt(), 12);
	EXPECT_DOUBLE_EQ(len.pc(), 1);

	len.set(2.54, "cm");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.cm(), 2.54);
	EXPECT_DOUBLE_EQ(len.mm(), 25.4);

	len.set(10, "mm");
	EXPECT_DOUBLE_EQ(len.cm(), 1);
	EXPECT_DOUBLE_EQ(len.mm(), 10);

	len.set(1, "dd");
	EXPECT_DOUBLE_EQ(len.pt(), 1238.0/1157);

	len.set(1, "cc");
	EXPECT_DOUBLE_EQ(len.dd(), 12.0);

	len.set(1, "sp");
	EXPECT_DOUBLE_EQ(len.pt(), 1.0/65536);

	len.set(1, "");
	EXPECT_DOUBLE_EQ(len.pt(), 1);

	ASSERT_THROW(len.set(1, "xy"), UnitException);
}

TEST(LengthTest, set3) {
	Length len;
	len.set("1pt");
	EXPECT_DOUBLE_EQ(len.pt(), 1);

	len.set("72bp");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.bp(), 72);
	EXPECT_DOUBLE_EQ(len.in(), 1);

	len.set("1in");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);

	len.set("1pc");
	EXPECT_DOUBLE_EQ(len.pt(), 12);
	EXPECT_DOUBLE_EQ(len.pc(), 1);

	len.set("2.54cm");
	EXPECT_DOUBLE_EQ(len.pt(), 72.27);
	EXPECT_DOUBLE_EQ(len.cm(), 2.54);
	EXPECT_DOUBLE_EQ(len.mm(), 25.4);

	len.set("10mm");
	EXPECT_DOUBLE_EQ(len.cm(), 1);
	EXPECT_DOUBLE_EQ(len.mm(), 10);

	len.set("10dd");
	EXPECT_DOUBLE_EQ(len.pt(), 12380.0/1157);

	len.set("10cc");
	EXPECT_DOUBLE_EQ(len.dd(), 120.0);

	len.set("10sp");
	EXPECT_DOUBLE_EQ(len.pt(), 10.0/65536);


	len.set("1");
	EXPECT_DOUBLE_EQ(len.pt(), 1);

	len.set("10");
	EXPECT_DOUBLE_EQ(len.pt(), 10);

	ASSERT_THROW(len.set("1xy"), UnitException);
	ASSERT_THROW(len.set("pt"), UnitException);
}


TEST(LengthTest, str2unit) {
	EXPECT_EQ(Length::stringToUnit("pt"), Length::PT);
	EXPECT_EQ(Length::stringToUnit("bp"), Length::BP);
	EXPECT_EQ(Length::stringToUnit("in"), Length::IN);
	EXPECT_EQ(Length::stringToUnit("cm"), Length::CM);
	EXPECT_EQ(Length::stringToUnit("mm"), Length::MM);
	EXPECT_EQ(Length::stringToUnit("pc"), Length::PC);
	EXPECT_EQ(Length::stringToUnit("cc"), Length::CC);
	EXPECT_EQ(Length::stringToUnit("dd"), Length::DD);
	EXPECT_EQ(Length::stringToUnit("sp"), Length::SP);

	ASSERT_THROW(Length::stringToUnit(""), UnitException);
	ASSERT_THROW(Length::stringToUnit("CM"), UnitException);
	ASSERT_THROW(Length::stringToUnit("pta"), UnitException);
}


TEST(LengthTest, unit2str) {
	EXPECT_EQ(Length::unitToString(Length::PT), string("pt"));
	EXPECT_EQ(Length::unitToString(Length::BP), string("bp"));
	EXPECT_EQ(Length::unitToString(Length::IN), string("in"));
	EXPECT_EQ(Length::unitToString(Length::CM), string("cm"));
	EXPECT_EQ(Length::unitToString(Length::MM), string("mm"));
	EXPECT_EQ(Length::unitToString(Length::PC), string("pc"));
	EXPECT_EQ(Length::unitToString(Length::CC), string("cc"));
	EXPECT_EQ(Length::unitToString(Length::DD), string("dd"));
	EXPECT_EQ(Length::unitToString(Length::SP), string("sp"));

	EXPECT_EQ(Length::unitToString(Length::Unit(300)), string("??"));
}

