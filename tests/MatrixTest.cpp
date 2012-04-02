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
#include <vector>
#include "Matrix.h"

using namespace std;


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


TEST(MatrixTest, scale) {
	ScalingMatrix m(2,2);
	DPair p = m*DPair(3,3);
	EXPECT_DOUBLE_EQ(p.x(), 6);
	EXPECT_DOUBLE_EQ(p.y(), 6);

	m = ScalingMatrix(-2,-2);
	p = m*DPair(3,3);
	EXPECT_DOUBLE_EQ(p.x(), -6);
	EXPECT_DOUBLE_EQ(p.y(), -6);
}


TEST(MatrixTest, rotate) {
	RotationMatrix m(90);
	DPair p = m*DPair(2,0);
	EXPECT_NEAR(p.x(), 0, 0.0000000001);
	EXPECT_NEAR(p.y(), 2, 0.0000000001);

	p = m*p;
	EXPECT_NEAR(p.x(), -2, 0.0000000001);
	EXPECT_NEAR(p.y(), 0, 0.0000000001);
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


TEST(MatrixTest, vec) {
	vector<double> v;
	for (int i=1; i <= 15; ++i)
		v.push_back(i);
	Matrix m(v);
	ostringstream oss;
	m.write(oss);
	EXPECT_EQ(oss.str(), "((1,2,3),(4,5,6),(7,8,9))");
	oss.str("");

	m.set(v, 2);
	m.write(oss);
	EXPECT_EQ(oss.str(), "((3,4,5),(6,7,8),(9,10,11))");
}


TEST(MatrixTest, det) {
	Matrix m1(1);
	EXPECT_EQ(det(m1), 1);

	double v2[] = {1,2,3, 4,5,6, 7,8,9};
	Matrix m2(v2);
	EXPECT_EQ(det(m2), 1*5*9 + 2*6*7 + 3*4*8 - 3*5*7 - 2*4*9 - 1*6*8);

	EXPECT_EQ(det(m2, 0, 0), 5*9-6*8);
	EXPECT_EQ(det(m2, 0, 1), 4*9-6*7);
	EXPECT_EQ(det(m2, 0, 2), 4*8-5*7);
	EXPECT_EQ(det(m2, 1, 0), 2*9-3*8);

	double v3[] = {1,1,1, 2,2,2, 3,3,3};
	Matrix m3(v3);
	EXPECT_EQ(det(m2), 0);
	m3.transpose();
	EXPECT_EQ(det(m2), 0);
}


TEST(MatrixTest, invert) {
	Matrix m1(1);
	EXPECT_EQ(m1.invert(), m1);

	double v2[] = {1,2,3, 2,3,1, 3,1,2};
	Matrix m2(v2);
	EXPECT_EQ(det(m2), -18);

	double v3[] = {5,-1,-7, -1,-7,5, -7,5,-1};
	Matrix m3(v3);

	m3 *= 1.0/det(m2);
	m2.invert();
	for (int i=0; i < 3; ++i)
		for (int j=0; j < 3; ++j)
			EXPECT_DOUBLE_EQ(m2.get(i,j), m3.get(i,j));
}
