/*************************************************************************
** ColorSpecialTest.cpp                                                 **
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
#include <sstream>
#include "ColorSpecialHandler.hpp"
#include "SpecialActions.hpp"

using namespace std;

class ColorSpecialTest : public ::testing::Test {
	protected:
		struct SetColor : EmptySpecialActions {
			void setFillColor (const Color &c) override   {fillColor = uint32_t(c);}
			void setStrokeColor (const Color &c) override {strokeColor = uint32_t(c);}
			bool equals (uint32_t c) const                {return fillColor == c && strokeColor == c;}
			bool equals (uint32_t fc, uint32_t sc) const  {return fillColor == fc && strokeColor == sc;}
			uint32_t fillColor=0, strokeColor=0;
		};
		ColorSpecialHandler handler;
		SetColor actions;
};


TEST_F(ColorSpecialTest, info) {
	EXPECT_EQ(handler.name(), "color");
	EXPECT_EQ(handler.prefixes().size(), 1u);
	EXPECT_STREQ(handler.prefixes()[0], "color");
	ASSERT_NE(handler.info(), nullptr);
	EXPECT_FALSE(string(handler.info()).empty());
}


TEST_F(ColorSpecialTest, readColor) {
	std::istringstream iss("rgb 1 0 1");
	EXPECT_EQ(ColorSpecialHandler::readColor(iss).rgbString(), "#f0f");
	iss.clear();
	iss.str("hsb 1 0.5 1");
	EXPECT_EQ(ColorSpecialHandler::readColor(iss).rgbString(), "#ff8080");
	iss.clear();
	iss.str("0 1 0");
	EXPECT_EQ(ColorSpecialHandler::readColor("rgb", iss).rgbString(), "#0f0");
	iss.clear();
	iss.str("1 0.5 1");
	EXPECT_EQ(ColorSpecialHandler::readColor("hsb", iss).rgbString(), "#ff8080");
}


TEST_F(ColorSpecialTest, gray) {
	std::istringstream iss("gray 0.2");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x333333));
}


TEST_F(ColorSpecialTest, rgb) {
	std::istringstream iss("rgb 1 0 1");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff00ff));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("fill rgb 1 0 0");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000, 0xff00ff));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("stroke rgb 0 1 1");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000, 0x00ffff));
	EXPECT_EQ(handler.stackSize(), 1u);
}


TEST_F(ColorSpecialTest, hsb) {
	std::istringstream iss("hsb 1 0.5 1");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff8080));
	EXPECT_EQ(handler.stackSize(), 1u);
}


TEST_F(ColorSpecialTest, cmyk) {
	std::istringstream iss("cmyk 0.1 0.2 0.4 0.6");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x1a336699));
	EXPECT_EQ(handler.stackSize(), 1u);
}


TEST_F(ColorSpecialTest, stack1) {
	std::istringstream iss("push rgb 1 0 0");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("push Blue");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x0000ff));
	EXPECT_EQ(handler.stackSize(), 2u);
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x000000));
	EXPECT_EQ(handler.stackSize(), 0u);
}


TEST_F(ColorSpecialTest, stack2) {
	std::istringstream iss("push rgb 1 0 0");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("push rgb 0 1 0");
	handler.process("", iss, actions);
	EXPECT_EQ(handler.stackSize(), 2u);
	iss.clear();
	iss.str("gray 0.2");  // clear color stack implicitly
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x333333));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x000000));
	EXPECT_EQ(handler.stackSize(), 0u);
}


TEST_F(ColorSpecialTest, stack3) {
	std::istringstream iss("push fill cmyk 0.1 0.2 0.4 0.6");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x1a336699, 0));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("push stroke Blue");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x1a336699, 0x0000ff));
	EXPECT_EQ(handler.stackSize(), 2u);
	iss.clear();
	iss.str("set fill Green");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x00ff00, 0x0000ff));
	EXPECT_EQ(handler.stackSize(), 2u);
	iss.clear();
	iss.str("set White");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xffffff));
	EXPECT_EQ(handler.stackSize(), 2u);
}


TEST_F(ColorSpecialTest, stack4) {
	std::istringstream iss("set stroke Cyan");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0, 0x00ffff));
	EXPECT_EQ(handler.stackSize(), 0u);
	iss.clear();
	iss.str("push fill Blue");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x0000ff, 0x00ffff));
	EXPECT_EQ(handler.stackSize(), 1u);
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0, 0x00ffff));
	EXPECT_EQ(handler.stackSize(), 0u);
}


TEST_F(ColorSpecialTest, constant) {
	std::istringstream iss("RedViolet");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x9600a8));
}


TEST_F(ColorSpecialTest, errors) {
	std::istringstream iss("UnknownColor");
	EXPECT_THROW(handler.process("", iss, actions), SpecialException);
	iss.clear();
	iss.str("blue");
	EXPECT_THROW(handler.process("", iss, actions), SpecialException);
	iss.clear();
	iss.str("rgb black");
	EXPECT_THROW(handler.process("", iss, actions), SpecialException);
}

