/*************************************************************************
** ColorSpecialTest.cpp                                                 **
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
#include <sstream>
#include "ColorSpecialHandler.h"
#include "SpecialActions.h"

class ColorSpecialTest : public ::testing::Test
{
	protected:
		struct SetColor : SpecialEmptyActions
		{
			SetColor () : color(0) {}
			void setColor (const Color &c) {color = c;}
			bool equals (UInt32 c) {return color == c;}
			UInt32 color;
		};
		ColorSpecialHandler handler;
		SetColor actions;
};


TEST_F(ColorSpecialTest, rgb) {
	std::istringstream iss("rgb 1 0 1");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0xff00ff));
}


TEST_F(ColorSpecialTest, hsb) {
	std::istringstream iss("hsb 1 0.5 1");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0xff8080));
}


TEST_F(ColorSpecialTest, cmyk) {
	std::istringstream iss("cmyk 0.1 0.2 0.4 0.6");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0x4c3300));
}


TEST_F(ColorSpecialTest, stack) {
	std::istringstream iss("push rgb 1 0 0");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0xff0000));
	iss.clear();
	iss.str("push Blue");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0x0000ff));
	iss.clear();
	iss.str("pop");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0xff0000));
}


TEST_F(ColorSpecialTest, constant) {
	std::istringstream iss("RedViolet");
	handler.process(0, iss, &actions);
	EXPECT_TRUE(actions.equals(0x9600a8));
}


TEST_F(ColorSpecialTest, errors) {
	std::istringstream iss("UnknownColor");
	EXPECT_THROW(handler.process(0, iss, &actions), SpecialException);
	iss.clear();
	iss.str("rgb 0 0.3 3.1");
	EXPECT_THROW(handler.process(0, iss, &actions), SpecialException);
}

