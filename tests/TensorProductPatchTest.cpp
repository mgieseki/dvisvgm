/*************************************************************************
** TensorProductPatchTest.cpp                                           **
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
#include <vector>
#include "Color.hpp"
#include "TensorProductPatch.hpp"

using namespace std;


#define EXPECT_NEAR_PAIR(name, p1, p2, eps) \
	{SCOPED_TRACE(name); expect_near_pair(p1, p2, eps);}

#define EXPECT_EQUAL_PAIR(name, p1, p2) \
	{SCOPED_TRACE(name); expect_equal_pair(p1, p2);}

#define CHECK_BEZIER_POINTS(name, bezier, p1, p2, p3, p4) \
	{SCOPED_TRACE(name); checkBezierPoints(bezier, p1, p2, p3, p4);}


static void expect_near_pair (const DPair &p1, const DPair &p2, double eps) {
	EXPECT_NEAR(p1.x(), p2.x(), eps);
	EXPECT_NEAR(p1.y(), p2.y(), eps);
}


static void expect_equal_pair (const DPair &p1, const DPair &p2) {
	EXPECT_DOUBLE_EQ(p1.x(), p2.x());
	EXPECT_DOUBLE_EQ(p1.y(), p2.y());
}


class TensorProductPatchTest : public ::testing::Test {
	protected:
		void SetUp () override {
			_points.resize(16);
			_points[0]  = DPair(10, 10);
			_points[1]  = DPair(0, 30);
			_points[2]  = DPair(20, 40);
			_points[3]  = DPair(10, 70);
			_points[4]  = DPair(20, 100);
			_points[5]  = DPair(70, 100);
			_points[6]  = DPair(100, 70);
			_points[7]  = DPair(90, 60);
			_points[8]  = DPair(80, 50);
			_points[9]  = DPair(70, 20);
			_points[10] = DPair(50, 30);
			_points[11] = DPair(20, 0);
			_points[12] = DPair(30, 40);
			_points[13] = DPair(40, 80);
			_points[14] = DPair(60, 70);
			_points[15] = DPair(40, 40);
			_patch.setPoints(_points, 0, 0);

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


	protected:
		vector<DPair> _points;
		TensorProductPatch _patch;
};


TEST_F(TensorProductPatchTest, construct) {
	EXPECT_EQ(_patch.psShadingType(), 7);
	EXPECT_EQ(_patch.numPoints(0), 16);
	EXPECT_EQ(_patch.numColors(0), 4);
	EXPECT_EQ(_patch.numPoints(1), 12);
	EXPECT_EQ(_patch.numColors(1), 2);

	CoonsPatch cp(Color::ColorSpace::RGB);
	EXPECT_EQ(cp.psShadingType(), 6);
	EXPECT_EQ(cp.numPoints(0), 12);
	EXPECT_EQ(cp.numColors(0), 4);
	EXPECT_EQ(cp.numPoints(1), 8);
	EXPECT_EQ(cp.numColors(1), 2);
}


TEST_F(TensorProductPatchTest, valueAt) {
	EXPECT_EQ(_patch.valueAt(0, 0), DPair(10, 10));
	EXPECT_EQ(_patch.valueAt(1, 0), DPair(70, 20));
	EXPECT_EQ(_patch.valueAt(0, 1), DPair(10, 70));
	EXPECT_EQ(_patch.valueAt(1, 1), DPair(100, 70));

	vector<DPair> points = _points;
	vector<Color> colors(2);
	points.resize(12);
	TensorProductPatch tpp1(points, colors, Color::ColorSpace::RGB, 1, &_patch);
	EXPECT_EQ(tpp1.valueAt(0, 0), DPair(10, 70));
	EXPECT_EQ(tpp1.valueAt(0, 1), DPair(100, 70));
	EXPECT_EQ(tpp1.valueAt(1, 0), DPair(70, 100));
	EXPECT_EQ(tpp1.valueAt(1, 1), DPair(20, 40));

	TensorProductPatch tpp2(points, colors, Color::ColorSpace::RGB, 2, &_patch);
	EXPECT_EQ(tpp2.valueAt(0, 0), DPair(100, 70));
	EXPECT_EQ(tpp2.valueAt(0, 1), DPair(70, 20));
	EXPECT_EQ(tpp2.valueAt(1, 0), DPair(70, 100));
	EXPECT_EQ(tpp2.valueAt(1, 1), DPair(20, 40));

	TensorProductPatch tpp3(points, colors, Color::ColorSpace::RGB, 3, &_patch);
	EXPECT_EQ(tpp3.valueAt(0, 0), DPair(70, 20));
	EXPECT_EQ(tpp3.valueAt(0, 1), DPair(10, 10));
	EXPECT_EQ(tpp3.valueAt(1, 0), DPair(70, 100));
	EXPECT_EQ(tpp3.valueAt(1, 1), DPair(20, 40));

	colors.resize(4);
	CoonsPatch cp1(points, colors, Color::ColorSpace::RGB, 0, 0);
	EXPECT_EQ(cp1.valueAt(0, 0), DPair(10, 70));
	EXPECT_EQ(cp1.valueAt(0, 1), DPair(10, 10));
	EXPECT_EQ(cp1.valueAt(1, 0), DPair(100, 70));
	EXPECT_EQ(cp1.valueAt(1, 1), DPair(70, 20));

	points.resize(8);
	colors.resize(2);
	CoonsPatch cp2(points, colors, Color::ColorSpace::RGB, 1, &cp1);
	EXPECT_EQ(cp2.valueAt(0, 0), DPair(100, 70));
	EXPECT_EQ(cp2.valueAt(0, 1), DPair(10, 70));
	EXPECT_EQ(cp2.valueAt(1, 0), DPair(20, 40));
	EXPECT_EQ(cp2.valueAt(1, 1), DPair(70, 100));

	CoonsPatch cp3(points, colors, Color::ColorSpace::RGB, 2, &cp1);
	EXPECT_EQ(cp3.valueAt(0, 0), DPair(70, 20));
	EXPECT_EQ(cp3.valueAt(0, 1), DPair(100, 70));
	EXPECT_EQ(cp3.valueAt(1, 0), DPair(20, 40));
	EXPECT_EQ(cp3.valueAt(1, 1), DPair(70, 100));

	CoonsPatch cp4(points, colors, Color::ColorSpace::RGB, 3, &cp1);
	EXPECT_EQ(cp4.valueAt(0, 0), DPair(10, 10));
	EXPECT_EQ(cp4.valueAt(0, 1), DPair(70, 20));
	EXPECT_EQ(cp4.valueAt(1, 0), DPair(20, 40));
	EXPECT_EQ(cp4.valueAt(1, 1), DPair(70, 100));
}


TEST_F(TensorProductPatchTest, averageColor) {
	EXPECT_EQ(_patch.averageColor().rgbString(), "#bf8040");
}



TEST_F(TensorProductPatchTest, vertices) {
	EXPECT_EQ(_patch.valueAt(0,0), DPair(10,10));
	EXPECT_EQ(_patch.valueAt(0,1), DPair(10,70));
	EXPECT_EQ(_patch.valueAt(1,1), DPair(100,70));
	EXPECT_EQ(_patch.valueAt(1,0), DPair(70,20));

	EXPECT_EQ(_patch.colorAt(0,0).rgbString(), "#f00");
	EXPECT_EQ(_patch.colorAt(0,1).rgbString(), "#ff0");
	EXPECT_EQ(_patch.colorAt(1,1).rgbString(), "#f0f");
	EXPECT_EQ(_patch.colorAt(1,0).rgbString(), "#0f0");
}


TEST_F(TensorProductPatchTest, curves) {
	Bezier bezier;
	_patch.horizontalCurve(0, bezier);
	CHECK_BEZIER_POINTS("A", bezier, DPair(10, 10), DPair(20, 0), DPair(50, 30), DPair(70, 20));
	_patch.horizontalCurve(1, bezier);
	CHECK_BEZIER_POINTS("B", bezier, DPair(10, 70), DPair(20, 100), DPair(70, 100), DPair(100, 70));
	_patch.verticalCurve(0, bezier);
	CHECK_BEZIER_POINTS("C", bezier, DPair(10, 10), DPair(0, 30), DPair(20, 40), DPair(10, 70));
	_patch.verticalCurve(1, bezier);
	CHECK_BEZIER_POINTS("D", bezier, DPair(70, 20), DPair(80, 50), DPair(90, 60), DPair(100, 70));
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
	EXPECT_EQUAL_PAIR("A", _patch.valueAt(0.25, 0.5), _patch.blossomValue(0.25, 0.25, 0.25, 0.5, 0.5, 0.5));
	EXPECT_NEAR_PAIR("B", _patch.valueAt(0.25, 0.5), DPair(26.1133, 48.457), 0.0001);

	EXPECT_EQUAL_PAIR("C", _patch.valueAt(0.8, 0.2), _patch.blossomValue(0.8, 0.8, 0.8, 0.2, 0.2, 0.2));
	EXPECT_NEAR_PAIR("D", _patch.valueAt(0.8, 0.2), DPair(59.5974, 35.4502), 0.0001);
}


TEST_F(TensorProductPatchTest, boundaryPath) {
	GraphicsPath<double> path = _patch.getBoundaryPath();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10C20 0 50 30 70 20C80 50 90 60 100 70C70 100 20 100 10 70C20 40 0 30 10 10Z");
}


TEST_F(TensorProductPatchTest, subpatch) {
	TensorProductPatch tpp;
	_patch.subpatch(0, 0.5, 0, 0.5, tpp);
	GraphicsPath<double> path = tpp.getBoundaryPath();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10C5 20 7.5 27.5 10 36.25C20.625 46.875 31.25 52.1875 43.28125 54.21875C40 40.9375 36.25 27.5 36.25 15C25 10 15 5 10 10Z");
	EXPECT_EQ(tpp.colorAt(0, 0).rgbString(), "#f00");
	EXPECT_EQ(tpp.colorAt(0, 1).rgbString(), "#ff8000");
	EXPECT_EQ(tpp.colorAt(1, 0).rgbString(), "#808000");
	EXPECT_EQ(tpp.colorAt(1, 1).rgbString(), "#bf8040");
}


TEST_F(TensorProductPatchTest, bbox) {
	BoundingBox bbox = _patch.getBBox();
	EXPECT_NEAR(bbox.minX(), 7.1132, 0.0001);
	EXPECT_NEAR(bbox.minY(), 7.9289, 0.0001);
	EXPECT_DOUBLE_EQ(bbox.maxX(), 100.0);
	EXPECT_DOUBLE_EQ(bbox.maxY(), 92.5);
}


class Callback : public ShadingPatch::Callback {
	public:
		void patchSegment (GraphicsPath<double> &path, const Color &color) {
			ostringstream oss;
			path.writeSVG(oss, false);
			_pathstr += oss.str();
			_colorstr += color.rgbString();
		}
		string pathstr() const  {return _pathstr;}
		string colorstr() const {return _colorstr;}
		void reset ()           {_pathstr.clear(); _colorstr.clear();}

	private:
		string _pathstr;
		string _colorstr;
};


TEST_F(TensorProductPatchTest, approximate) {
	Callback callback;
	vector<Color> colors(4);
	TensorProductPatch tpp(_points, colors, Color::ColorSpace::RGB, 0, 0);
	tpp.approximate(2, false, 0.1, callback);
	EXPECT_EQ(callback.pathstr(), "M10 10C20 0 50 30 70 20C80 50 90 60 100 70C70 100 20 100 10 70C20 40 0 30 10 10Z");
	EXPECT_EQ(callback.colorstr(), "#000");

	callback.reset();
	_patch.approximate(2, false, 0.1, callback);
	EXPECT_EQ(
		callback.pathstr(),
	   "M10 10C15 5 25 10 36.25 15C36.25 27.5 40 40.9375 43.28125 54.21875C31.25 52.1875 20.625 46.875 10 36.25C7.5 27.5 5 20 10 10Z"
		"M36.25 15C47.5 20 60 25 70 20C75 35 80 45 85 52.5C68.75 55 55.3125 56.25 43.28125 54.21875C40 40.9375 36.25 27.5 36.25 15Z"
		"M10 36.25C20.625 46.875 31.25 52.1875 43.28125 54.21875C46.5625 67.5 49.375 80.625 47.5 92.5C30 92.5 15 85 10 70C15 55 12.5 45 10 36.25Z"
		"M43.28125 54.21875C55.3125 56.25 68.75 55 85 52.5C90 60 95 65 100 70C85 85 65 92.5 47.5 92.5C49.375 80.625 46.5625 67.5 43.28125 54.21875Z");
	EXPECT_EQ(callback.colorstr(), "#cf6010#70a030#efa030#cf6090");
}



TEST_F(TensorProductPatchTest, fail) {
	// edge flag == 0
	vector<DPair> points(15);
	EXPECT_THROW(_patch.setPoints(points, 0, 0), ShadingException);
	points.resize(17);  // too many points
	EXPECT_THROW(_patch.setPoints(points, 0, 0), ShadingException);

	vector<Color> colors(2); // too few colors
	EXPECT_THROW(_patch.setColors(colors, 0, 0), ShadingException);
	colors.resize(5);  // too many colors
	EXPECT_THROW(_patch.setColors(colors, 0, 0), ShadingException);

	// edge flag > 0
	points.resize(16);
	EXPECT_THROW(_patch.setPoints(points, 1, 0), ShadingException);
	points.resize(11);  // too few points
	EXPECT_THROW(_patch.setPoints(points, 1, &_patch), ShadingException);
	points.resize(13);  // too many points
	EXPECT_THROW(_patch.setPoints(points, 1, &_patch), ShadingException);

	colors.resize(4);
	EXPECT_THROW(_patch.setColors(colors, 1, 0), ShadingException);
	colors.resize(1);  // too few colors
	EXPECT_THROW(_patch.setColors(colors, 1, &_patch), ShadingException);
	colors.resize(3);  // too many colors
	EXPECT_THROW(_patch.setColors(colors, 1, &_patch), ShadingException);

	CoonsPatch cp;
	points.resize(8);
	EXPECT_THROW(cp.setPoints(points, 1, 0), ShadingException);
	points.resize(11);
	EXPECT_THROW(cp.setPoints(points, 0, 0), ShadingException);
	colors.resize(2);
	EXPECT_THROW(cp.setColors(colors, 1, 0), ShadingException);
	colors.resize(5);
	EXPECT_THROW(cp.setColors(colors, 0, 0), ShadingException);
}
