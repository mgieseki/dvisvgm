/*************************************************************************
** MatrixTest.cpp                                                       **
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
#include "Matrix.h"

using std::ostringstream;

TEST(MatrixTest, svg) {
	double v1[] = {1,2,3,4,5,6,7,8,9};
	Matrix m1(v1);
	ostringstream oss;
	m1.write(oss);
	EXPECT_EQ(oss.str(), "((1,2,3),(4,5,6),(7,8,9))");
	EXPECT_EQ(m1.getSVG(), "matrix(1 4 2 5 3 6)");

	double v2[] = {1,2};
	Matrix m2(v2, 2);
	oss.str("");
	m2.write(oss);
	EXPECT_EQ(oss.str(), "((1,2,0),(0,1,0),(0,0,1))");
	EXPECT_EQ(m2.getSVG(), "matrix(1 0 2 1 0 0)");
}


TEST(MatrixTest, transpose) {
	double v[] = {1,2,3,4,5,6,7,8,9};
	Matrix m(v);
	m.transpose();
	ostringstream oss;
	m.write(oss);
	EXPECT_EQ(oss.str(), "((1,4,7),(2,5,8),(3,6,9))");
	EXPECT_EQ(m.getSVG(), "matrix(1 2 4 5 7 8)");
}


TEST(MatrixTest, checks) {
	Matrix m(1);
	EXPECT_TRUE(m.isIdentity());
	double tx, ty;
	EXPECT_TRUE(m.isTranslation(tx, ty));
	EXPECT_EQ(tx, 0);
	EXPECT_EQ(ty, 0);
	m.translate(1,2);
	EXPECT_TRUE(m.isTranslation(tx, ty));
	EXPECT_EQ(tx, 1);
	EXPECT_EQ(ty, 2);
	m.scale(2, 2);
	EXPECT_FALSE(m.isTranslation(tx, ty));
}
