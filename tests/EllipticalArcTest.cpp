/*************************************************************************
** EllipticalArcTest.cpp                                                **
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
#include "EllipticalArc.hpp"
#include "utility.hpp"

using namespace std;

#define EXPECT_NEAR_PAIR(p1, p2, eps) \
	EXPECT_NEAR(p1.x(), p2.x(), eps); \
	EXPECT_NEAR(p1.y(), p2.y(), eps)


TEST(EllipticalArcTest, construct1) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 1, 1, DPair(225, 125));
	EXPECT_FALSE(arc.isStraightLine());
	EXPECT_EQ(arc.startPoint(), DPair(125,75));
	EXPECT_EQ(arc.endPoint(), DPair(225,125));
	EXPECT_EQ(arc.rx(), 100);
	EXPECT_EQ(arc.ry(), 50);
	EXPECT_NEAR(math::rad2deg(arc.rotationAngle()), 30, 0.0001);
	EXPECT_TRUE(arc.largeArc());
	EXPECT_TRUE(arc.sweepPositive());
}


TEST(EllipticalArcTest, construct2) {
	// radii two small, check automatic adaption
	EllipticalArc arc(DPair(125,75), 20, 10, math::deg2rad(30), 1, 1, DPair(225, 125));
	EXPECT_FALSE(arc.isStraightLine());
	EXPECT_EQ(arc.startPoint(), DPair(125,75));
	EXPECT_EQ(arc.endPoint(), DPair(225,125));
	EXPECT_NEAR(arc.rx(), 56.2, 0.01);
	EXPECT_NEAR(arc.ry(), 28.1, 0.01);
	EXPECT_NEAR(math::rad2deg(arc.rotationAngle()), 30, 0.0001);
	EXPECT_TRUE(arc.largeArc());
	EXPECT_TRUE(arc.sweepPositive());
}


TEST(EllipticalArcTest, construct3) {
	EXPECT_TRUE(EllipticalArc(DPair(125,75), 0, 10, math::deg2rad(30), 1, 1, DPair(225, 125)).isStraightLine());
	EXPECT_TRUE(EllipticalArc(DPair(125,75), 20, 0, math::deg2rad(30), 1, 1, DPair(225, 125)).isStraightLine());
	EXPECT_TRUE(EllipticalArc(DPair(125,75), 0, 0, math::deg2rad(30), 1, 1, DPair(225, 125)).isStraightLine());
}


TEST(EllipticalArcTest, construct4) {
	// center parameterization
	EllipticalArc arc(DPair(100,100), 100, 50, math::deg2rad(30), math::deg2rad(30), math::deg2rad(80));
	EXPECT_FALSE(arc.isStraightLine());
	EXPECT_NEAR_PAIR(arc.startPoint(), DPair(152.452, 164.952), 0.001);
	EXPECT_NEAR_PAIR(arc.endPoint(), DPair(50.856, 123.589), 0.001);
	EXPECT_NEAR(arc.rx(), 100, 0.1);
	EXPECT_NEAR(arc.ry(), 50, 0.1);
	EXPECT_NEAR(math::rad2deg(arc.rotationAngle()), 30, 0.0001);
	EXPECT_FALSE(arc.largeArc());
	EXPECT_TRUE(arc.sweepPositive());
}


TEST(EllipticalArcTest, transform) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 1, 1, DPair(225, 125));
	Matrix m(1);
	m.scale(1,2).rotate(30).xskewByAngle(15).rotate(20).yskewByAngle(-20);
	arc.transform(m);
	EXPECT_NEAR_PAIR(arc.startPoint(), DPair(13.8871, 204.752), 0.001);
	EXPECT_NEAR_PAIR(arc.endPoint(), DPair(35.957, 350.121), 0.001);
	EXPECT_NEAR(arc.rx(), 154.73, 0.001);
	EXPECT_NEAR(arc.ry(), 64.629, 0.001);
	EXPECT_NEAR(math::rad2deg(arc.rotationAngle()), -81.748, 0.001);
	EXPECT_TRUE(arc.largeArc());
	EXPECT_TRUE(arc.sweepPositive());
}


TEST(EllipticalArcTest, bbox1) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 1, 1, DPair(225, 125));
	BoundingBox bbox = arc.getBBox();
	EXPECT_NEAR(bbox.minX(), 96.854, 0.001);
	EXPECT_NEAR(bbox.minY(), -6.633, 0.001);
	EXPECT_NEAR(bbox.width(), 180.278, 0.001);
  	EXPECT_NEAR(bbox.height(), 132.288, 0.001);
}


TEST(EllipticalArcTest, bbox2) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 0, 1, DPair(225, 125));
	BoundingBox bbox = arc.getBBox();
	EXPECT_NEAR(bbox.minX(), 125, 0.1);
	EXPECT_NEAR(bbox.minY(), 75, 0.1);
	EXPECT_NEAR(bbox.width(), 100, 0.1);
  	EXPECT_NEAR(bbox.height(), 50, 0.1);
}


TEST(EllipticalArcTest, approximate1) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 1, 1, DPair(225, 125));
	auto beziers = arc.approximate();
	ASSERT_EQ(beziers.size(), 4u);
	DPair data[4][4] = {
		{DPair(125, 75),         DPair(96.271, 46.768),   DPair(88.655, 17.402),   DPair(106.223, 2.602)},
		{DPair(106.223, 2.602),  DPair(123.791, -12.198), DPair(162.434, -8.971),  DPair(201.493, 10.558)},
		{DPair(201.493, 10.558), DPair(240.551, 30.088),  DPair(270.89, 61.351),   DPair(276.289, 87.635)},
		{DPair(276.289, 87.635), DPair(281.688, 113.919), DPair(260.884, 129.074), DPair(225, 125)}
	};
	for (int i=0; i < 4; i++) {
		for (int j=0; j < 4; j++) {
			EXPECT_NEAR_PAIR(beziers[i].point(j), data[i][j], 0.001);
		}
	}
}


TEST(EllipticalArcTest, approximate2) {
	EllipticalArc arc(DPair(125,75), 100, 50, math::deg2rad(30), 0, 1, DPair(225, 125));
	auto beziers = arc.approximate();
	ASSERT_EQ(beziers.size(), 1u);
	DPair data[] = {DPair(125, 75), DPair(158.522, 78.806), DPair(198.163, 98.627), DPair(225, 125)};
	for (int j=0; j < 4; j++) {
		EXPECT_NEAR_PAIR(beziers[0].point(j), data[j], 0.001);
	}
}
