/*************************************************************************
** BezierTest.cpp                                                       **
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
#include "Bezier.hpp"

using namespace std;

const double EPS = 0.001;

#define EXPECT_PAIR_EQ(p1,p2) {EXPECT_DOUBLE_EQ(p1.x(), p2.x()); EXPECT_DOUBLE_EQ(p1.y(), p2.y());}
#define EXPECT_PAIR_NEAR(p1,p2) {EXPECT_NEAR(p1.x(), p2.x(), EPS); EXPECT_NEAR(p1.y(), p2.y(), EPS);}
#define EXPECT_BBOX_NEAR(b1,b2) { \
	EXPECT_NEAR(b1.minX(), b2.minX(), EPS); EXPECT_NEAR(b1.maxX(), b2.maxX(), EPS); \
	EXPECT_NEAR(b1.minY(), b2.minY(), EPS); EXPECT_NEAR(b1.maxY(), b2.maxY(), EPS);}


TEST(BezierTest, construct) {
	// quadratic Bézier curve
	Bezier bezier1(DPair(0,0), DPair(12,12), DPair(24,6));
	EXPECT_EQ(bezier1.point(0), DPair(0,0));
	EXPECT_EQ(bezier1.point(1), DPair(8,8));
	EXPECT_EQ(bezier1.point(2), DPair(16,10));
	EXPECT_EQ(bezier1.point(3), DPair(24,6));

	// cubic Bézier curve
	Bezier bezier2(DPair(0,0), DPair(12,12), DPair(24,6), DPair(19,-4));
	EXPECT_EQ(bezier2.point(0), DPair(0,0));
	EXPECT_EQ(bezier2.point(1), DPair(12,12));
	EXPECT_EQ(bezier2.point(2), DPair(24,6));
	EXPECT_EQ(bezier2.point(3), DPair(19,-4));
}


TEST(BezierTest, subcurve) {
	Bezier bezier1(DPair(0,0), DPair(12,12), DPair(24,6), DPair(19,-4));
	{
		Bezier subcurve(bezier1, 0, 1);
		EXPECT_EQ(subcurve.point(0), DPair(0,0));
		EXPECT_EQ(subcurve.point(1), DPair(12,12));
		EXPECT_EQ(subcurve.point(2), DPair(24,6));
		EXPECT_EQ(subcurve.point(3), DPair(19,-4));
	}
	{
		Bezier subcurve(bezier1, 0, 0.5);
		EXPECT_EQ(subcurve.point(0), DPair(0,0));
		EXPECT_EQ(subcurve.point(1), DPair(6,6));
		EXPECT_EQ(subcurve.point(2), DPair(12,7.5));
		EXPECT_EQ(subcurve.point(3), DPair(15.875,6.25));
	}
	{
		Bezier subcurve(bezier1, 1, 0.5);
		EXPECT_EQ(subcurve.point(0), DPair(15.875,6.25));
		EXPECT_EQ(subcurve.point(1), DPair(19.75,5));
		EXPECT_EQ(subcurve.point(2), DPair(21.5,1));
		EXPECT_EQ(subcurve.point(3), DPair(19,-4));
	}
	{
		Bezier subcurve(bezier1, 0.2, 0.8);
		EXPECT_PAIR_NEAR(subcurve.point(0), DPair(7.064,5.152));
		EXPECT_PAIR_NEAR(subcurve.point(1), DPair(13.856,8.368));
		EXPECT_PAIR_NEAR(subcurve.point(2), DPair(19.424,6.112));
		EXPECT_PAIR_NEAR(subcurve.point(3), DPair(20.096,1.408));
	}
	{
		Bezier subcurve(bezier1, 0, 0);
		EXPECT_EQ(subcurve.point(0), DPair(0,0));
		EXPECT_EQ(subcurve.point(1), DPair(0,0));
		EXPECT_EQ(subcurve.point(2), DPair(0,0));
		EXPECT_EQ(subcurve.point(3), DPair(0,0));
	}
	{
		Bezier subcurve(bezier1, 1, 1);
		EXPECT_EQ(subcurve.point(0), DPair(19,-4));
		EXPECT_EQ(subcurve.point(1), DPair(19,-4));
		EXPECT_EQ(subcurve.point(2), DPair(19,-4));
		EXPECT_EQ(subcurve.point(3), DPair(19,-4));
	}
}


TEST(BezierTest, reverse) {
	Bezier bezier(DPair(0,0), DPair(12,12), DPair(24,6), DPair(19,-4));
	bezier.reverse();
	EXPECT_EQ(bezier.point(0), DPair(19,-4));
	EXPECT_EQ(bezier.point(1), DPair(24,6));
	EXPECT_EQ(bezier.point(2), DPair(12,12));
	EXPECT_EQ(bezier.point(3), DPair(0,0));
}


TEST(BezierTest, bbox) {
	Bezier bezier(DPair(0,0), DPair(12,12), DPair(24,6), DPair(30,-5));
	EXPECT_BBOX_NEAR(bezier.getBBox(), BoundingBox(0, -5, 30, 6.598));
}


TEST(BezierTest, approximate) {
	vector<DPair> points;
	vector<double> times;
	Bezier bezier(DPair(0,0), DPair(12,12), DPair(24,6), DPair(30,-5));
	size_t size = bezier.approximate(0.1, points, &times);
	double t[] = {0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 1};
	DPair p[] = {
		DPair(0,0), DPair(4.488,3.681), DPair(8.906,5.828), DPair(13.183,6.591),
		DPair(17.25,6.125), DPair(21.035,4.580), DPair(24.468,2.109), DPair(30,-5)
	};
	EXPECT_EQ(size, (size_t)sizeof(t)/sizeof(double));
	EXPECT_EQ(points.size(), size);
	EXPECT_EQ(times.size(), size);
	for (size_t i=0; i < size; i++) {
		EXPECT_EQ(times[i], t[i]) << "i=" << i;
		EXPECT_PAIR_NEAR(points[i], p[i]);
	}
}


TEST(BezierTest, reduceDegree) {
	vector<DPair> points;
	Bezier bezier(DPair(0,0), DPair(5,10), DPair(10,5));
	int degree = bezier.reduceDegree(0.1, points);
	EXPECT_EQ(degree, 2);
	EXPECT_EQ(points[0], DPair(0,0));
	EXPECT_EQ(points[1], DPair(5,10));
	EXPECT_EQ(points[2], DPair(10,5));

	bezier.setPoints(DPair(3,8), DPair(3,8), DPair(3,8), DPair(3,8));
	degree = bezier.reduceDegree(0.1, points);
	EXPECT_EQ(degree, 0);
	EXPECT_EQ(points[0], DPair(3,8));

	bezier.setPoints(DPair(0,0), DPair(5,5), DPair(8,8), DPair(10,10));
	degree = bezier.reduceDegree(0.1, points);
	EXPECT_EQ(degree, 1);
	EXPECT_EQ(points[0], DPair(0,0));
	EXPECT_EQ(points[1], DPair(10,10));

	bezier.setPoints(DPair(0,0), DPair(5,10), DPair(10,5), DPair(15,10));
	degree = bezier.reduceDegree(0.1, points);
	EXPECT_EQ(degree, 3);
	EXPECT_EQ(points[0], DPair(0,0));
	EXPECT_EQ(points[1], DPair(5,10));
	EXPECT_EQ(points[2], DPair(10,5));
	EXPECT_EQ(points[3], DPair(15,10));
}
