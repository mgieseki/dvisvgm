/*************************************************************************
** GraphicPathTest.cpp                                                  **
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
#include "GraphicPath.h"

using std::ostringstream;

TEST(GraphicPathTest, svg) {
	GraphicPath<int> path;
	path.moveto(0,0);
	path.lineto(10,10);
	path.cubicto(20,20,30,30,40,40);
	path.closepath();
	ostringstream oss;
	path.writeSVG(oss);
	EXPECT_EQ(oss.str(), "M0 0L10 10C20 20 30 30 40 40Z");
}


TEST(GraphicPathTest, optimize) {
	GraphicPath<int> path;
	path.moveto(0,0);
	path.lineto(10,0);
	path.lineto(10,20);
	ostringstream oss;
	path.writeSVG(oss);
	EXPECT_EQ(oss.str(), "M0 0H10V20");
}


TEST(GraphicPathTest, transform) {
	GraphicPath<double> path;
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
	path.writeSVG(oss);
	EXPECT_EQ(oss.str(), "M-100 10V12H-102V10Z");
}


TEST(GraphicPathTest, closeOpenSubPaths) {
	GraphicPath<double> path;
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
	path.writeSVG(oss);
	EXPECT_EQ(oss.str(), "M0 0H1V1H0ZM10 10H11V11H10Z");
}
