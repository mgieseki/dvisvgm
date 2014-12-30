/*************************************************************************
** TensorProductPatchTest.cpp                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <vector>
#include "Color.h"
#include "TensorProductPatch.h"

using namespace std;


class TensorProductPatchTest : public ::testing::Test
{
	protected:
		void SetUp () {
			vector<DPair> points(16);
			points[0]  = DPair(10, 10);
			points[1]  = DPair(0, 30);
			points[2]  = DPair(20, 40);
			points[3]  = DPair(10, 70);
			points[4]  = DPair(20, 100);
			points[5]  = DPair(70, 100);
			points[6]  = DPair(100, 70);
			points[7]  = DPair(90, 60);
			points[8]  = DPair(80, 50);
			points[9]  = DPair(70, 20);
			points[10] = DPair(50, 30);
			points[11] = DPair(20, 0);
			points[12] = DPair(30, 40);
			points[13] = DPair(40, 80);
			points[14] = DPair(60, 70);
			points[15] = DPair(40, 40);
			_patch.setPoints(points, 0, 0);

			vector<Color> colors(4);
			colors[0].setRGB(1.0, 0.0, 0.0);
			colors[1].setRGB(1.0, 1.0, 0.0);
			colors[2].setRGB(1.0, 0.0, 1.0);
			colors[3].setRGB(0.0, 1.0, 0.0);
			_patch.setColors(colors, 0, 0);
		}

		void checkBezierPoints (const Bezier &b, const DPair &p1, const DPair &p2, const DPair &p3, const DPair &p4) const {
			EXPECT_EQ(b.point(0), p1);
			EXPECT_EQ(b.point(1), p2);
			EXPECT_EQ(b.point(2), p3);
			EXPECT_EQ(b.point(3), p4);
		}

		void expectNear (const DPair &p1, const DPair &p2, double eps) const {
			EXPECT_NEAR(p1.x(), p2.x(), eps);
			EXPECT_NEAR(p1.y(), p2.y(), eps);
		}

		void expectEqual (const DPair &p1, const DPair &p2) const {
			EXPECT_DOUBLE_EQ(p1.x(), p2.x());
			EXPECT_DOUBLE_EQ(p1.y(), p2.y());
		}


	protected:
		TensorProductPatch _patch;
};


TEST_F(TensorProductPatchTest, vertices) {
	EXPECT_EQ(_patch.valueAt(0,0), DPair(10,10));
	EXPECT_EQ(_patch.valueAt(0,1), DPair(10,70));
	EXPECT_EQ(_patch.valueAt(1,1), DPair(100,70));
	EXPECT_EQ(_patch.valueAt(1,0), DPair(70,20));

	EXPECT_EQ(_patch.colorAt(0,0).rgbString(), "#ff0000");
	EXPECT_EQ(_patch.colorAt(0,1).rgbString(), "#ffff00");
	EXPECT_EQ(_patch.colorAt(1,1).rgbString(), "#ff00ff");
	EXPECT_EQ(_patch.colorAt(1,0).rgbString(), "#00ff00");
}


TEST_F(TensorProductPatchTest, curves) {
	Bezier bezier;
	_patch.horizontalCurve(0, bezier);
	checkBezierPoints(bezier, DPair(10, 10), DPair(20, 0), DPair(50, 30), DPair(70, 20));
	_patch.horizontalCurve(1, bezier);
	checkBezierPoints(bezier, DPair(10, 70), DPair(20, 100), DPair(70, 100), DPair(100, 70));
	_patch.verticalCurve(0, bezier);
	checkBezierPoints(bezier, DPair(10, 10), DPair(0, 30), DPair(20, 40), DPair(10, 70));
	_patch.verticalCurve(1, bezier);
	checkBezierPoints(bezier, DPair(70, 20), DPair(80, 50), DPair(90, 60), DPair(100, 70));
}


TEST_F(TensorProductPatchTest, blossom_outer) {
	EXPECT_EQ(_patch.blossomValue(0,0,0,0,0,0), DPair(10, 10));
	EXPECT_EQ(_patch.blossomValue(0,0,0,1,0,0), DPair(0, 30));
	EXPECT_EQ(_patch.blossomValue(0,0,0,1,1,0), DPair(20, 40));
	EXPECT_EQ(_patch.blossomValue(0,0,0,1,1,1), DPair(10, 70));

	EXPECT_EQ(_patch.blossomValue(1,1,1,0,0,0), DPair(70, 20));
	EXPECT_EQ(_patch.blossomValue(1,1,1,1,0,0), DPair(80, 50));
	EXPECT_EQ(_patch.blossomValue(1,1,1,1,1,0), DPair(90, 60));
	EXPECT_EQ(_patch.blossomValue(1,1,1,1,1,1), DPair(100, 70));

	EXPECT_EQ(_patch.blossomValue(1,0,0,0,0,0), DPair(20, 0));
	EXPECT_EQ(_patch.blossomValue(1,1,0,0,0,0), DPair(50, 30));
	EXPECT_EQ(_patch.blossomValue(1,1,1,0,0,0), DPair(70, 20));

	EXPECT_EQ(_patch.blossomValue(0,0,0,1,1,1), DPair(10, 70));
	EXPECT_EQ(_patch.blossomValue(1,0,0,1,1,1), DPair(20, 100));
	EXPECT_EQ(_patch.blossomValue(1,1,0,1,1,1), DPair(70, 100));
}


TEST_F(TensorProductPatchTest, blossom_inner) {
	EXPECT_EQ(_patch.blossomValue(1,0,0,1,0,0), DPair(30, 40));
	EXPECT_EQ(_patch.blossomValue(1,1,0,1,0,0), DPair(40, 40));
	EXPECT_EQ(_patch.blossomValue(1,0,0,1,1,0), DPair(40, 80));
	EXPECT_EQ(_patch.blossomValue(1,1,0,1,1,0), DPair(60, 70));
}


TEST_F(TensorProductPatchTest, values) {
	expectEqual(_patch.valueAt(0.25, 0.5), _patch.blossomValue(0.25, 0.25, 0.25, 0.5, 0.5, 0.5));
	expectNear(_patch.valueAt(0.25, 0.5), DPair(26.1133, 48.457), 0.0001);

	expectEqual(_patch.valueAt(0.8, 0.2), _patch.blossomValue(0.8, 0.8, 0.8, 0.2, 0.2, 0.2));
	expectNear(_patch.valueAt(0.8, 0.2), DPair(59.5974, 35.4502), 0.0001);
}


TEST_F(TensorProductPatchTest, fail) {
	// edge flag == 0
	vector<DPair> points(15); // too few points
	EXPECT_THROW(_patch.setPoints(points, 0, 0), ShadingException);
	points.resize(17);  // too many points
	EXPECT_THROW(_patch.setPoints(points, 0, 0), ShadingException);

	vector<Color> colors(2); // too few colors
	EXPECT_THROW(_patch.setColors(colors, 0, 0), ShadingException);
	colors.resize(5);  // too many colors
	EXPECT_THROW(_patch.setColors(colors, 0, 0), ShadingException);

	// edge flag > 0
	points.resize(11);  // too few points
	EXPECT_THROW(_patch.setPoints(points, 1, &_patch), ShadingException);
	points.resize(13);  // too many points
	EXPECT_THROW(_patch.setPoints(points, 1, &_patch), ShadingException);

	colors.resize(1);  // too few colors
	EXPECT_THROW(_patch.setColors(colors, 1, &_patch), ShadingException);
	colors.resize(3);  // too many colors
	EXPECT_THROW(_patch.setColors(colors, 1, &_patch), ShadingException);
}
