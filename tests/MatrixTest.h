/***********************************************************************
** MatrixTest.h                                                       **
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
#include "Matrix.h"

using std::ostringstream;

class MatrixTest : public CxxTest::TestSuite
{
	public:
		void test_svg () {
			double v1[] = {1,2,3,4,5,6,7,8,9};
			Matrix m1(v1);
			ostringstream oss;
			m1.write(oss);
			TS_ASSERT_EQUALS(oss.str(), "((1,2,3),(4,5,6),(7,8,9))");
			TS_ASSERT_EQUALS(m1.getSVG(), "matrix(1 4 2 5 3 6)");

			double v2[] = {1,2};
			Matrix m2(v2, 2);
			oss.str("");
			m2.write(oss);
			TS_ASSERT_EQUALS(oss.str(), "((1,2,0),(0,1,0),(0,0,1))");
			TS_ASSERT_EQUALS(m2.getSVG(), "matrix(1 0 2 1 0 0)");
		}


		void test_transpose () {
			double v[] = {1,2,3,4,5,6,7,8,9};
			Matrix m(v);
			m.transpose();
			ostringstream oss;
			m.write(oss);
			TS_ASSERT_EQUALS(oss.str(), "((1,4,7),(2,5,8),(3,6,9))");
			TS_ASSERT_EQUALS(m.getSVG(), "matrix(1 2 4 5 7 8)");
		}

		
		void test_checks () {
			Matrix m(1);
			TS_ASSERT(m.isIdentity());
			double tx, ty;
			TS_ASSERT(m.isTranslation(tx, ty));
			TS_ASSERT_EQUALS(tx, 0);
			TS_ASSERT_EQUALS(ty, 0);
			m.translate(1,2);
			TS_ASSERT(m.isTranslation(tx, ty));
			TS_ASSERT_EQUALS(tx, 1);
			TS_ASSERT_EQUALS(ty, 2);
			m.scale(2, 2);
			TS_ASSERT(!m.isTranslation(tx, ty));
		}
};
