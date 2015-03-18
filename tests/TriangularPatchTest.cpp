/*************************************************************************
** XMLStringTest.cpp                                                    **
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
#include "TriangularPatch.h"

using namespace std;


TEST(TriangularPatchTest, construct) {
	TriangularPatch tp1(Color::RGB_SPACE);
	EXPECT_EQ(tp1.psShadingType(), 4);
	EXPECT_EQ(tp1.numPoints(0), 3);
	EXPECT_EQ(tp1.numColors(0), 3);
	EXPECT_EQ(tp1.numPoints(1), 1);
	EXPECT_EQ(tp1.numColors(1), 1);

	vector<DPair> points(1);
	points[0] = DPair(10, 0);
	vector<Color> colors(1);
	TriangularPatch tp2(points, colors, Color::RGB_SPACE, 1, &tp1);
	EXPECT_EQ(tp1.psShadingType(), 4);

	LatticeTriangularPatch tp3(Color::RGB_SPACE);
	EXPECT_EQ(tp3.psShadingType(), 5);
}


TEST(TriangularPatchTest, pointAt) {
	vector<DPair> points(3);
	points[0] = DPair(0, 0);
	points[1] = DPair(10, 0);
	points[2] = DPair(0, 10);
	vector<Color> colors(3);
	TriangularPatch tp1(points, colors, Color::RGB_SPACE, 0, 0);
	EXPECT_EQ(tp1.pointAt(0, 0), DPair(0,0));
	EXPECT_EQ(tp1.pointAt(1, 0), DPair(10,0));
	EXPECT_EQ(tp1.pointAt(0, 1), DPair(0,10));

	points.resize(1);
	colors.resize(1);
	points[0] = DPair(10, 10);
	TriangularPatch tp2(points, colors, Color::RGB_SPACE, 1, &tp1);
	EXPECT_EQ(tp2.pointAt(0, 0), DPair(10,10));
	EXPECT_EQ(tp2.pointAt(1, 0), DPair(10,0));
	EXPECT_EQ(tp2.pointAt(0, 1), DPair(0,10));

	TriangularPatch tp3(points, colors, Color::RGB_SPACE, 2, &tp1);
	EXPECT_EQ(tp3.pointAt(0, 0), DPair(10,10));
	EXPECT_EQ(tp3.pointAt(1, 0), DPair(0,10));
	EXPECT_EQ(tp3.pointAt(0, 1), DPair(0,0));

	tp1.setPoints(DPair(20,20), DPair(20,40), DPair(40,20));
	EXPECT_EQ(tp1.pointAt(0, 0), DPair(20,20));
	EXPECT_EQ(tp1.pointAt(1, 0), DPair(20,40));
	EXPECT_EQ(tp1.pointAt(0, 1), DPair(40,20));
}


TEST(TriangularPatchTest, colorAt) {
	vector<DPair> points(3);
	vector<Color> colors(3);
	colors[0] = Color(1.0, 0.0, 0.0);
	colors[1] = Color(0.0, 1.0, 0.0);
	colors[2] = Color(0.0, 0.0, 1.0);
	TriangularPatch tp1(points, colors, Color::RGB_SPACE, 0, 0);
	EXPECT_EQ(tp1.colorAt(0, 0), Color(1.0, 0.0, 0.0));
	EXPECT_EQ(tp1.colorAt(1, 0), Color(0.0, 1.0, 0.0));
	EXPECT_EQ(tp1.colorAt(0, 1), Color(0.0, 0.0, 1.0));

	points.resize(1);
	colors.resize(1);
	colors[0] = Color(1.0, 1.0, 0.0);
	TriangularPatch tp2(points, colors, Color::RGB_SPACE, 1, &tp1);
	EXPECT_EQ(tp2.colorAt(0, 0), Color(1.0, 1.0, 0.0));
	EXPECT_EQ(tp2.colorAt(1, 0), Color(0.0, 1.0, 0.0));
	EXPECT_EQ(tp2.colorAt(0, 1), Color(0.0, 0.0, 1.0));

	TriangularPatch tp3(points, colors, Color::RGB_SPACE, 2, &tp1);
	EXPECT_EQ(tp3.colorAt(0, 0), Color(1.0, 1.0, 0.0));
	EXPECT_EQ(tp3.colorAt(1, 0), Color(0.0, 0.0, 1.0));
	EXPECT_EQ(tp3.colorAt(0, 1), Color(1.0, 0.0, 0.0));

	tp1.setColors(Color(1.0, 1.0, 0.0), Color(1.0, 0.0, 1.0), Color(0.0, 1.0, 1.0));
	EXPECT_EQ(tp1.colorAt(0, 0), Color(1.0, 1.0, 0.0));
	EXPECT_EQ(tp1.colorAt(1, 0), Color(1.0, 0.0, 1.0));
	EXPECT_EQ(tp1.colorAt(0, 1), Color(0.0, 1.0, 1.0));
}


TEST(TriangularPatchTest, averageColor) {
	vector<DPair> points(3);
	vector<Color> colors(3);
	colors[0] = Color(1.0, 0.0, 0.0);
	colors[1] = Color(0.0, 1.0, 0.0);
	colors[2] = Color(0.0, 0.0, 1.0);
	TriangularPatch tp(points, colors, Color::RGB_SPACE, 0, 0);
	EXPECT_EQ(tp.averageColor(), Color(UInt8(85), UInt8(85), UInt8(85)));
}


TEST(TriangularPatchTest, bbox) {
	vector<DPair> points(3);
	points[0] = DPair(0, 0);
	points[1] = DPair(10, 0);
	points[2] = DPair(0, 10);
	vector<Color> colors(3);
	TriangularPatch tp(points, colors, Color::RGB_SPACE, 0, 0);
	BoundingBox bbox;
	tp.getBBox(bbox);
	EXPECT_EQ(bbox, BoundingBox(0, 0, 10, 10));
}


TEST(TriangularPatchTest, boundaryPath) {
	vector<DPair> points(3);
	points[0] = DPair(0, 0);
	points[1] = DPair(10, 0);
	points[2] = DPair(0, 10);
	vector<Color> colors(3);
	TriangularPatch tp(points, colors, Color::RGB_SPACE, 0, 0);
	GraphicPath<double> path;
	tp.getBoundaryPath(path);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0H10L0 10Z");
}


class Callback : public ShadingPatch::Callback {
	public:
		void patchSegment (GraphicPath<double> &path, const Color &color) {
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


TEST(TriangularPatchTest, approximate) {
	vector<DPair> points(3);
	points[0] = DPair(0, 0);
	points[1] = DPair(10, 0);
	points[2] = DPair(0, 10);
	vector<Color> colors(3);
	TriangularPatch tp(points, colors, Color::RGB_SPACE, 0, 0);
	Callback callback;
	tp.approximate(2, false, 0.1, callback);
	EXPECT_EQ(callback.pathstr(), "M0 0H10L0 10Z");
	EXPECT_EQ(callback.colorstr(), "#000000");

	callback.reset();
	tp.setColors(Color(1.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), Color(0.0, 0.0, 1.0));
	tp.approximate(2, false, 0.1, callback);
	EXPECT_EQ(callback.pathstr(), "M0 0H5L0 5ZM0 5L5 0V5ZM0 5H5L0 10ZM5 0H10L5 5Z");
	EXPECT_EQ(callback.colorstr(), "#aa2b2b#555555#2b2baa#2baa2b");
}


TEST(TriangularPatchTest, fail) {
	vector<DPair> points(3);
	vector<Color> colors(3);
	TriangularPatch tp(points, colors, Color::RGB_SPACE, 0, 0);
	EXPECT_THROW(tp.setColors(colors, 1, 0), ShadingException);

	for (int i=1; i <= 3; i++) {
		EXPECT_THROW(TriangularPatch(points, colors, Color::RGB_SPACE, i, 0), ShadingException);
	}
	points.resize(2);
	EXPECT_THROW(TriangularPatch(points, colors, Color::RGB_SPACE, 0, 0), ShadingException);
	points.resize(3);
	colors.resize(2);
	EXPECT_THROW(TriangularPatch(points, colors, Color::RGB_SPACE, 0, 0), ShadingException);
}