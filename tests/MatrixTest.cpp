/*************************************************************************
** MatrixTest.cpp                                                       **
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
#include <vector>
#include <XMLString.hpp>
#include "Calculator.hpp"
#include "Matrix.hpp"

using namespace std;


TEST(MatrixTest, construct1) {
	const vector<double> vec{1, 2, 3, 4, 5, 6, 7, 8, 9};
	Matrix m1(vec);
	for (int row=0; row < 3; row++)
		for (int col=0; col < 3; col++)
			ASSERT_EQ(m1.get(row, col), vec[row*3+col]) << "row=" << row << ", col=" << col;

	int startIndex=4;
	Matrix m2(vec, startIndex);
	for (int row=0; row < 3; row++) {
		for (int col=0; col < 3; col++) {
			int index = row*3+col+startIndex;
			ASSERT_EQ(m2.get(row, col), index < 9 ? vec[index] : (row == col ? 1 : 0)) << "row=" << row << ", col=" << col;
		}
	}
	EXPECT_TRUE(Matrix(1).isIdentity());
}


TEST(MatrixTest, construct2) {
	Matrix m1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	for (int row=0; row < 3; row++)
		for (int col=0; col < 3; col++)
			ASSERT_EQ(m1.get(row, col), 3*row+col+1) << "row=" << row << ", col=" << col;

	Matrix m2 = {1, 2, 3, 4, 5, 6};
	for (int row=0; row < 2; row++)
		for (int col=0; col < 3; col++)
			ASSERT_EQ(m2.get(row, col), 3*row+col+1) << "row=" << row << ", col=" << col;
	ASSERT_EQ(m2.get(2, 0), 0);
	ASSERT_EQ(m2.get(2, 1), 0);
	ASSERT_EQ(m2.get(2, 2), 1);
}


TEST(MatrixTest, svg) {
	double v1[] = {1,2,3,4,5,6,7,8,9};
	Matrix m1(v1);
	ostringstream oss;
	m1.write(oss);
	EXPECT_EQ(oss.str(), "((1,2,3),(4,5,6),(7,8,9))");
	EXPECT_EQ(m1.toSVG(), "matrix(1 4 2 5 3 6)");

	double v2[] = {1,2};
	Matrix m2(v2, 2);
	oss.str("");
	m2.write(oss);
	EXPECT_EQ(oss.str(), "((1,2,0),(0,1,0),(0,0,1))");
	EXPECT_EQ(m2.toSVG(), "matrix(1 0 2 1 0 0)");
}


TEST(MatrixTest, transpose) {
	double v[] = {1,2,3,4,5,6,7,8,9};
	Matrix m(v);
	m.transpose();
	ostringstream oss;
	m.write(oss);
	EXPECT_EQ(oss.str(), "((1,4,7),(2,5,8),(3,6,9))");
	EXPECT_EQ(m.toSVG(), "matrix(1 2 4 5 7 8)");
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


TEST(MatrixTest, isTranslation) {
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


TEST(MatrixTest, rmultiply) {
	const Matrix m1({1, 2, 3, 4, 5, 6, 7, 8, 9});
	const Matrix m2({9, 8, 7, 6, 5, 4, 3, 2, 1});
	EXPECT_NE(m1, m2);
	Matrix m3;
	EXPECT_EQ((m3 = m1).rmultiply(m2), Matrix({30, 24, 18, 84, 69, 54, 138, 114, 90}));
	EXPECT_EQ((m3 = m2).rmultiply(m1), Matrix({90, 114, 138, 54, 69, 84, 18, 24, 30}));
	EXPECT_EQ((m3 = m1).rmultiply(Matrix(1)), m1);
}


TEST(MatrixTest, lmultiply) {
	const Matrix m1({1, 2, 3, 4, 5, 6, 7, 8, 9});
	const Matrix m2({9, 8, 7, 6, 5, 4, 3, 2, 1});
	EXPECT_NE(m1, m2);
	Matrix m3;
	EXPECT_EQ((m3 = m1).lmultiply(m2), Matrix({90, 114, 138, 54, 69, 84, 18, 24, 30}));
	EXPECT_EQ((m3 = m2).lmultiply(m1), Matrix({30, 24, 18, 84, 69, 54, 138, 114, 90}));
	EXPECT_EQ((m3 = m1).lmultiply(Matrix(1)), m1);
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
	Matrix m4{0};
	EXPECT_THROW(m4.invert(), exception);
}


TEST(MatrixTest, parse) {
	Calculator calc;
	calc.setVariable("ux", 0);
	calc.setVariable("uy", 0);
	calc.setVariable("w", 0);
	calc.setVariable("h", 0);
	EXPECT_EQ(Matrix("T1,-2", calc), TranslationMatrix(1, -2));
	EXPECT_EQ(Matrix("R45", calc), RotationMatrix(45));
	EXPECT_EQ(Matrix("S2,3", calc), ScalingMatrix(2, 3));
	EXPECT_EQ(Matrix("FH1", calc), Matrix({1, 0, 0, 0, -1, 2, 0, 0, 1}));
	EXPECT_EQ(Matrix("FV1", calc), Matrix({-1, 0, 2, 0, 1, 0, 0, 0, 1}));
	EXPECT_EQ(Matrix("KX45", calc), Matrix({1, 1, 0, 0, 1, 0, 0, 0, 1}));
	EXPECT_EQ(Matrix("KY45", calc), Matrix({1, 0, 0, 1, 1, 0, 0, 0, 1}));
	EXPECT_EQ(Matrix("M1,2,3,4,5,6", calc), Matrix({1, 2, 3, 4, 5, 6, 0, 0, 1}));

	Matrix m;
	m.set("R90 T1,1 S2", calc);
	EXPECT_EQ(m, Matrix({0, -2, 2, 2, 0, 2, 0, 0, 1}));
}


TEST(MatrixTest, write) {
	ostringstream oss;
	Matrix m(3);
	oss << m;
	EXPECT_EQ(oss.str(), "((3,0,0),(0,3,0),(0,0,3))");
}


TEST(MatrixTest, fail) {
	Calculator calc;
	EXPECT_THROW(Matrix("R45", calc), CalculatorException);

	calc.setVariable("ux", 0);
	calc.setVariable("uy", 0);
	calc.setVariable("w", 0);
	calc.setVariable("h", 0);
	EXPECT_THROW(Matrix("ABC", calc), ParserException);   // invalid command
	EXPECT_THROW(Matrix("F1", calc), ParserException);    // missing H or V
	EXPECT_THROW(Matrix("K45", calc), ParserException);   // missing X or Y
	EXPECT_THROW(Matrix("KX", calc), ParserException);    // missing argument
	EXPECT_THROW(Matrix("KX90", calc), ParserException);  // invalid argument (pole at 90+180k degrees)
	EXPECT_THROW(Matrix("KY270", calc), ParserException); // invalid argument (pole at 90+180k degrees)
	EXPECT_THROW(Matrix("S2,", calc), ParserException);   // missing argument
}


TEST(MatrixTest, parseSVGTransform) {
	XMLString::DECIMAL_PLACES = 3;
	EXPECT_EQ(
		Matrix::parseSVGTransform("translate(50, 90)").toSVG(),
		"matrix(1 0 0 1 50 90)");
	EXPECT_EQ(
		Matrix::parseSVGTransform("scale(10 20)").toSVG(),
		"matrix(10 0 0 20 0 0)");
	EXPECT_EQ(
		Matrix::parseSVGTransform("rotate(-45)").toSVG(),
		"matrix(.707 -.707 .707 .707 0 0)");
	EXPECT_EQ(
		Matrix::parseSVGTransform("translate(50, 90) rotate(-45) ").toSVG(),
		"matrix(.707 -.707 .707 .707 50 90)");
	EXPECT_EQ(
		Matrix::parseSVGTransform("translate(50, 90) rotate(-45) , translate(130 160)").toSVG(),
		"matrix(.707 -.707 .707 .707 255.061 111.213)");
}
