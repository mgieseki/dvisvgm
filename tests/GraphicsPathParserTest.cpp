/*************************************************************************
** GraphicsPathParserTest.cpp                                           **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2025 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "GraphicsPathParser.hpp"

using namespace std;

TEST(GraphicsPathParserTest, empty) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("");
	EXPECT_TRUE(path.empty());
	path = parser.parse("   \n\t \r ");
	EXPECT_TRUE(path.empty());
}


TEST(GraphicsPathParserTest, moveto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("M10 10 20 20 ,30 0 -10 10");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10L20 20L30 0L-10 10");
	oss.str("");
	path = parser.parse("m10 -10 20, 20 ,30 0 -10 10");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10-10L30 10H60L50 20");
}


TEST(GraphicsPathParserTest, lineto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("L10 10 20 20 ,30 0 -10 10");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "L10 10L20 20L30 0L-10 10");
	oss.str("");
	path = parser.parse("l10 -10 20, 20 ,30 0 -10 10");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "L10-10L30 10H60L50 20");
}


TEST(GraphicsPathParserTest, hvlineto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("H 10 20 V10 20");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "H10H20V10V20");
	oss.str("");
	path = parser.parse("h 10 20 v10 20");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "H10H30V10V30");
}


TEST(GraphicsPathParserTest, cubicto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("C 10 20 30 40 50 60 -100 -200 -300 -400 -500 -600");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "C10 20 30 40 50 60C-100-200-300-400-500-600");
	oss.str("");
	path = parser.parse("c 10 20 30 40 50 60 -100 -200 -300 -400 -500 -600");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "C10 20 30 40 50 60C-50-140-250-340-450-540");
}


TEST(GraphicsPathParserTest, quadto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("Q 10 20 30 40 -100 -200 -300 -400");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "Q10 20 30 40Q-100-200-300-400");
	oss.str("");
	path = parser.parse("q 10 20 30 40 -100 -200 -300 -400");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "Q10 20 30 40Q-70-160-270-360");
}


TEST(GraphicsPathParserTest, scubicto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("S 10 20 30 40 -100 -200 -300 -400");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "C0 0 10 20 30 40S-100-200-300-400");
	oss.str("");
	path = parser.parse("s 10 20 30 40 -100 -200 -300 -400");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "C0 0 10 20 30 40S-70-160-270-360");
}


TEST(GraphicsPathParserTest, squadto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("T 10 20 -100 -200");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "Q0 0 10 20T-100-200");
	oss.str("");
	path = parser.parse("t 10 20 -100 -200");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "Q0 0 10 20T-90-180");
}


TEST(GraphicsPathParserTest, arcto) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("A 10 20 90 1 0 100 200, -10 -20 45 0 1 -100 -200");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "A10 20 90 1 0 100 200A10 20 45 0 1-100-200");
	oss.str("");
	path = parser.parse("a 10 20 90 1 0 100 200, -10 -20 45 0 1 -100 -200");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "A10 20 90 1 0 100 200A10 20 45 0 1 0 0");
}


TEST(GraphicsPathParserTest, combined) {
	GraphicsPathParser<int> parser;
	auto path = parser.parse("M10 10L20 50Q 100 100 -10 -10 Z C 10 20 30 40 50 60Z");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10L20 50Q100 100-10-10ZC10 20 30 40 50 60Z");
	oss.str("");
	path = parser.parse("m10 10l20 50q 100 100 -10 -10 z c 10 20 30 40 50 60 z");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10L30 60Q130 160 20 50ZC20 30 40 50 60 70Z");
}


TEST(GraphicsPathParserTest, floats) {
	GraphicsPathParser<double> parser;
	auto path = parser.parse("M10 10.1L20.2 50.3Q 100 100 -10.1 -10.2 Z C 10e-1 2e2 30 40 50 60Z");
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10.1L20.2 50.3Q100 100-10.1-10.2ZC1 200 30 40 50 60Z");
	oss.str("");
	path = parser.parse("m10 10.1l20.2 50.3q 100 100 -10.1 -10.2 z c 10 20 30 40 50 60 z");
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10.1L30.2 60.4Q130.2 160.4 20.1 50.2ZC20 30.1 40 50.1 60 70.1Z");
}


TEST(GraphicsPathParserTest, error1) {
	GraphicsPathParser<int> parser;
	EXPECT_THROW(parser.parse("10 20"), GraphicsPathParserException);  // missing command
	EXPECT_THROW(parser.parse("M10 "), GraphicsPathParserException);  // missing y-coordinate
	EXPECT_THROW(parser.parse("M10 20.5"), GraphicsPathParserException); // invalid number type
	EXPECT_THROW(parser.parse("J 10 20.5"), GraphicsPathParserException); // unknown command
	EXPECT_THROW(parser.parse("M,10 20"), GraphicsPathParserException); // invalid comma
	EXPECT_THROW(parser.parse("M10 20,"), GraphicsPathParserException); // missing parameters
	EXPECT_THROW(parser.parse("A 10 20 45 2 0 100 100"), GraphicsPathParserException); // invalid large-arc-flag
	EXPECT_THROW(parser.parse("A 10 20 45 0 5 100 100"), GraphicsPathParserException); // invalid sweep-flag
}


TEST(GraphicsPathParserTest, error2) {
	GraphicsPathParser<double> parser;
	EXPECT_THROW(parser.parse("10 20"), GraphicsPathParserException);  // missing command
	EXPECT_THROW(parser.parse("M"), GraphicsPathParserException);  // missing coordinates
	EXPECT_THROW(parser.parse("M10.1 "), GraphicsPathParserException);  // missing y-coordinate
	EXPECT_THROW(parser.parse("M 10 20..5"), GraphicsPathParserException); // invalid double dots
	EXPECT_THROW(parser.parse("M 10 20.5."), GraphicsPathParserException); // invalid trailing dot
	EXPECT_THROW(parser.parse("M 10-20.1+"), GraphicsPathParserException); // invalid trailing plus
	EXPECT_THROW(parser.parse("M.+10.20"), GraphicsPathParserException); // invalid plus after dot
}
