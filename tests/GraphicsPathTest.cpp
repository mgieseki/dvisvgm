/*************************************************************************
** GraphicsPathTest.cpp                                                 **
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
#include "GraphicsPath.hpp"

using namespace std;

TEST(GraphicsPathTest, svg) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,10);
	path.cubicto(20,20,30,30,40,40);
	path.closepath();
	EXPECT_FALSE(path.empty());
	EXPECT_EQ(path.size(), 4u);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0L10 10C20 20 30 30 40 40Z");
	path.clear();
	EXPECT_TRUE(path.empty());
}


TEST(GraphicsPathTest, optimize) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,0);
	path.lineto(10,20);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0H10V20");
}


TEST(GraphicsPathTest, transform) {
	GraphicsPath<double> path;
	path.moveto(0,0);
	path.lineto(1,0);
	path.lineto(1,1);
	path.lineto(0,1);
	path.closepath();
	Matrix m(1);
	m.scale(2,2);
	m.translate(10, 100);
	m.rotate(90);
	path.transform(m);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M-100 10V12H-102V10Z");
}


TEST(GraphicsPathTest, closeOpenSubPaths) {
	GraphicsPath<double> path;
	path.moveto(0,0);
	path.lineto(1,0);
	path.lineto(1,1);
	path.lineto(0,1);
	path.moveto(10,10);
	path.lineto(11,10);
	path.lineto(11,11);
	path.lineto(10,11);
	path.closeOpenSubPaths();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0H1V1H0ZM10 10H11V11H10Z");
}


TEST(GraphicsPathTest, relative1) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,10);
	path.lineto(10,20);
	path.cubicto(20,20,30,30,40,40);
	path.quadto(50, 50, 60, 60);
	path.lineto(100,60);
	path.closepath();
	ostringstream oss;
	path.writeSVG(oss, true);
	EXPECT_EQ(oss.str(), "m0 0l10 10v10c10 0 20 10 30 20q10 10 20 20h40z");
}


TEST(GraphicsPathTest, computeBBox) {
	GraphicsPath<int> path;
	path.moveto(10,10);
	path.lineto(100,10);
	path.quadto(10, 100, 40, 80);
	path.cubicto(5,5,30,10,90,70);
	path.lineto(20,30);
	path.closepath();
	BoundingBox bbox = path.computeBBox();
	EXPECT_EQ(bbox, BoundingBox(5, 5, 100, 100));
}


TEST(GraphicsPathTest, removeRedundantCommands) {
	GraphicsPath<int> path;
	path.moveto(10,10);
	path.lineto(100,10);
	path.quadto(10, 100, 40, 80);
	path.cubicto(5,5,30,10,90,70);
	path.moveto(10,10);
	path.moveto(15,10);
	path.moveto(20,20);
	path.lineto(20,30);
	path.moveto(10,10);
	path.moveto(20,20);
	path.removeRedundantCommands();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10H100Q10 100 40 80C5 5 30 10 90 70M20 20V30");
}

TEST(GraphicsPathTest, cmd_equals) {
	using Point = Pair<int>;
	gp::MoveTo<int> m1(Point(1, 2));
	gp::MoveTo<int> m2(Point(1, 2));
	gp::MoveTo<int> m3(Point(2, 1));
	EXPECT_EQ(m1, m2);
	EXPECT_EQ(m2, m1);
	EXPECT_NE(m1, m3);
	EXPECT_NE(m3, m1);
	gp::LineTo<int> l1(Point(1, 2));
	EXPECT_NE(l1, m1);
	EXPECT_NE(m1, l1);
	gp::CubicTo<int> c1(Point(1, 2), Point(3, 4), Point(5, 6));
	gp::CubicTo<int> c2(Point(1, 2), Point(3, 4), Point(5, 6));
	gp::CubicTo<int> c3(Point(1, 2), Point(0, 4), Point(5, 6));
	EXPECT_EQ(c1, c2);
	EXPECT_EQ(c2, c1);
	EXPECT_NE(c1, c3);
	EXPECT_NE(c3, c1);
}


TEST(GraphicsPathTest, equals) {
	GraphicsPath<int> path1;
	EXPECT_TRUE(path1 == path1);
	path1.moveto(10,10);
	path1.lineto(100,10);
	path1.quadto(10, 100, 40, 80);
	path1.cubicto(5,5,30,10,90,70);
	path1.lineto(20,30);
	path1.closepath();
	EXPECT_TRUE(path1 == path1);

	GraphicsPath<int> path2;
	EXPECT_FALSE(path1 == path2);
	path2.moveto(10,10);
	path2.lineto(100,10);
	path2.quadto(10, 100, 40, 80);
	path2.cubicto(5,5,30,10,90,70);
	path2.lineto(20,30);
	EXPECT_FALSE(path1 == path2);
	EXPECT_FALSE(path2 == path1);
	path2.closepath();
	EXPECT_TRUE(path1 == path2);
	EXPECT_TRUE(path2 == path1);

	path2.clear();
	path2.moveto(10,10);
	path2.lineto(100,10);
	path2.quadto(10, 100, 40, 80);
	path2.cubicto(5,5,10,10,90,70);
	path2.lineto(20,30);
	path2.closepath();
	EXPECT_FALSE(path1 == path2);
	EXPECT_FALSE(path2 == path1);
}


TEST(GraphicsPathTest, unequals) {
	GraphicsPath<int> path1;
	EXPECT_FALSE(path1 != path1);
	path1.moveto(10,10);
	path1.lineto(100,10);
	path1.quadto(10, 100, 40, 80);
	path1.cubicto(5,5,30,10,90,70);
	path1.lineto(20,30);
	path1.closepath();
	EXPECT_FALSE(path1 != path1);

	GraphicsPath<int> path2;
	EXPECT_TRUE(path1 != path2);
	path2.moveto(10,10);
	path2.lineto(100,10);
	path2.quadto(10, 100, 40, 80);
	path2.cubicto(5,5,30,10,90,70);
	path2.lineto(20,30);
	EXPECT_TRUE(path1 != path2);
	EXPECT_TRUE(path2 != path1);
	path2.closepath();
	EXPECT_FALSE(path1 != path2);
	EXPECT_FALSE(path2 != path1);

	path2.clear();
	path2.moveto(10,10);
	path2.lineto(100,10);
	path2.quadto(10, 100, 40, 80);
	path2.cubicto(5,5,10,10,90,70);
	path2.lineto(20,30);
	path2.closepath();
	EXPECT_TRUE(path1 != path2);
	EXPECT_TRUE(path2 != path1);
}


TEST(GraphicsPathTest, approximate_arcs) {
	GraphicsPath<double> path;
	XMLString::DECIMAL_PLACES = 2;
	path.moveto(10, 10);
	path.lineto(20, 0);
	path.arcto(30, 20, 20, 1, 1, DPair(50, 50));
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10L20 0A30 20 20 1 1 50 50");
	path.approximateArcs();
	oss.str("");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10L20 0C25.05-7.15 34.02-8.12 42.72-2.44S58.14 14.42 59.73 25.91S57.48 46.9 50 50");
}


TEST(GraphicsPathTest, smooth_quadto) {
	GraphicsPath<int> path;
	path.moveto(10, 10);
	path.quadto(DPair(30, 20), DPair(40, 10));
	path.quadto(DPair(100, 30));
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10Q30 20 40 10T100 30");
}


TEST(GraphicsPathTest, smooth_cubicto) {
	GraphicsPath<int> path;
	path.moveto(10, 10);
	path.cubicto(DPair(30, 20), DPair(40, 0), DPair(20, 50));
	path.cubicto(DPair(80, 100), DPair(100, 30));
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10C30 20 40 0 20 50S80 100 100 30");
}
