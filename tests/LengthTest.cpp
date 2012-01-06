/*************************************************************************
** LengthTest.cpp                                                       **
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
#include "Length.h"

TEST(LengthTest, set1) {
	Length len;
	len.set(1, Length::PT);
	EXPECT_EQ(len.pt(), 1);
	
	len.set(72, Length::BP);
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.bp(), 72);
	EXPECT_EQ(len.in(), 1);

	len.set(1, Length::IN);
	EXPECT_EQ(len.pt(), 72.27);

	len.set(12, Length::PC);
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.in(), 1);

	len.set(2.54, Length::CM);
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.cm(), 2.54);
	EXPECT_EQ(len.mm(), 25.4);

	len.set(25.4, Length::MM);
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.cm(), 2.54);
	EXPECT_EQ(len.mm(), 25.4);
}


TEST(LengthTest, set2) {
	Length len;
	len.set(1, "pt");
	EXPECT_EQ(len.pt(), 1);
	
	len.set(72, "bp");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.bp(), 72);
	EXPECT_EQ(len.in(), 1);
	EXPECT_EQ(len.pc(), 12);

	len.set(1, "in");
	EXPECT_EQ(len.pt(), 72.27);

	len.set(12, "pc");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.in(), 1);
	EXPECT_EQ(len.pc(), 12);

	len.set(2.54, "cm");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.cm(), 2.54);
	EXPECT_EQ(len.mm(), 25.4);

	len.set(10, "mm");
	EXPECT_EQ(len.cm(), 1);
	EXPECT_EQ(len.mm(), 10);

	len.set(1, "");
	EXPECT_EQ(len.pt(), 1);

	ASSERT_THROW(len.set(1, "xy"), UnitException);
}

TEST(LengthTest, set3) {
	Length len;
	len.set("1pt");
	EXPECT_EQ(len.pt(), 1);
	
	len.set("72bp");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.bp(), 72);
	EXPECT_EQ(len.in(), 1);
	EXPECT_EQ(len.pc(), 12);

	len.set("1in");
	EXPECT_EQ(len.pt(), 72.27);

	len.set("12pc");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.in(), 1);
	EXPECT_EQ(len.pc(), 12);

	len.set("2.54cm");
	EXPECT_EQ(len.pt(), 72.27);
	EXPECT_EQ(len.cm(), 2.54);
	EXPECT_EQ(len.mm(), 25.4);

	len.set("10mm");
	EXPECT_EQ(len.cm(), 1);
	EXPECT_EQ(len.mm(), 10);

	len.set("1");
	EXPECT_EQ(len.pt(), 1);

	ASSERT_THROW(len.set("1xy"), UnitException);
	ASSERT_THROW(len.set("pt"), UnitException);
}
