/***********************************************************************
** CalculatorTest.h                                                   **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id: CalculatorTest.h,v 1.3 2006/01/06 13:32:41 mgieseki Exp $

#include <cxxtest/TestSuite.h>
#include "Calculator.h"

class CalculatorTest : public CxxTest::TestSuite
{
	public:
		void test_eval () {
			TS_ASSERT_EQUALS(calc.eval("2+3+4"),    9);
			TS_ASSERT_EQUALS(calc.eval("2*3+4"),   10);
			TS_ASSERT_EQUALS(calc.eval("2+3*4"),   14);
			TS_ASSERT_EQUALS(calc.eval("(2+3)*4"), 20);
			TS_ASSERT_EQUALS(calc.eval("2*(3+4)"), 14);
			TS_ASSERT_EQUALS(calc.eval("-2+3+4"),   5);
			TS_ASSERT_EQUALS(calc.eval("3/2"),    1.5);
			TS_ASSERT_EQUALS(calc.eval("3%2"),      1);
		}

		void test_variables () {
			calc.setVariable("a", 1);
			TS_ASSERT_EQUALS(calc.getVariable("a"), 1);
			
			calc.setVariable("a", 2);
			TS_ASSERT_EQUALS(calc.getVariable("a"), 2);
			
			calc.setVariable("b", 3);
			TS_ASSERT_EQUALS(calc.eval("a+b"), 5);
			TS_ASSERT_EQUALS(calc.eval("2a+2b"), 10);
		}

		void test_exceptions () {
			TS_ASSERT_THROWS(calc.eval("2++3"), CalculatorException);
			TS_ASSERT_THROWS(calc.eval("c"), CalculatorException);
			TS_ASSERT_THROWS(calc.eval("1/0"), CalculatorException);
			TS_ASSERT_THROWS(calc.eval("1%0"), CalculatorException);
			TS_ASSERT_THROWS(calc.eval("2*(3+4"), CalculatorException);
			TS_ASSERT_THROWS(calc.eval("2*(3+4))"), CalculatorException);
		}
		
	private:
		Calculator calc;
};
