/***********************************************************************
** ColorSpecialTest.h                                                 **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/

#include <cxxtest/TestSuite.h>
#include <sstream>
#include "ColorSpecialHandler.h"
#include "SpecialActions.h"

class ColorSpecialTest : public CxxTest::TestSuite
{
	struct SetColor : SpecialEmptyActions
	{
		SetColor () : color(0) {}
		void setColor (const Color &c) {color = c;}
		bool equals (UInt32 c) {return color == c;}
		UInt32 color;
	};

	public:
		void test_rgb () {
			std::istringstream iss("rgb 1 0 1");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0xff00ff));
		}

		void test_hsb () {
			std::istringstream iss("hsb 1 0.5 1");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0xff8080));
		}

		void test_cmyk () {
			std::istringstream iss("cmyk 0.1 0.2 0.4 0.6");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0x4c3300));
		}

		void test_stack () {
			std::istringstream iss("push rgb 1 0 0");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0xff0000));
			iss.clear();
			iss.str("push Blue");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0x0000ff));
			iss.clear();
			iss.str("pop");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0xff0000));
		}

		void test_constant () {
			std::istringstream iss("RedViolet");
			handler.process(0, iss, &actions);
			TS_ASSERT(actions.equals(0x9600a8));
		}

		void test_errors () {
			std::istringstream iss("UnknownColor");
			TS_ASSERT_THROWS(handler.process(0, iss, &actions), SpecialException);
			iss.clear();
			iss.str("rgb 0 0.3 3.1");
			TS_ASSERT_THROWS(handler.process(0, iss, &actions), SpecialException);
		}

	private:
		ColorSpecialHandler handler;
		SetColor actions;
};
