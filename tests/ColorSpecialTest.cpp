/*************************************************************************
** ColorSpecialTest.cpp                                                 **
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
#include "ColorSpecialHandler.hpp"
#include "SpecialActions.hpp"

using namespace std;

class ColorSpecialTest : public ::testing::Test {
	protected:
		struct SetColor : EmptySpecialActions {
			SetColor () : color(0) {}
			void setColor (const Color &c) {color = uint32_t(c);}
			bool equals (uint32_t c) {return color == c;}
			uint32_t color;
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
}


TEST_F(ColorSpecialTest, hsb) {
	std::istringstream iss("hsb 1 0.5 1");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff8080));
}


TEST_F(ColorSpecialTest, cmyk) {
	std::istringstream iss("cmyk 0.1 0.2 0.4 0.6");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x5c523d));
}


TEST_F(ColorSpecialTest, stack1) {
	std::istringstream iss("push rgb 1 0 0");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	iss.clear();
	iss.str("push Blue");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x0000ff));
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x000000));
}


TEST_F(ColorSpecialTest, stack2) {
	std::istringstream iss("push rgb 1 0 0");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	iss.clear();
	iss.str("push rgb 0 1 0");
	handler.process("", iss, actions);
	iss.clear();
	iss.str("gray 0.2");  // clear color stack implicitly
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x333333));
	iss.clear();
	iss.str("pop");
	handler.process("", iss, actions);
	EXPECT_TRUE(actions.equals(0x000000));
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

