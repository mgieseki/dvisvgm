/*************************************************************************
** BoundingBoxTest.cpp                                                  **
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

#define _USE_MATH_DEFINES
#include <gtest/gtest.h>
#include <cmath>
#include <sstream>
#include "BoundingBox.hpp"
#include "Length.hpp"
#include "Matrix.hpp"
#include "XMLNode.hpp"

using namespace std;

TEST(BoundingBoxTest, construct) {
	BoundingBox bbox1;
	EXPECT_FALSE(bbox1.valid());
	EXPECT_FALSE(bbox1.locked());

	BoundingBox bbox2(50, 20, 5, 2);
	EXPECT_TRUE(bbox2.valid());
	EXPECT_FALSE(bbox2.locked());
	EXPECT_DOUBLE_EQ(bbox2.minX(), 5);
	EXPECT_DOUBLE_EQ(bbox2.maxX(), 50);
	EXPECT_DOUBLE_EQ(bbox2.minY(), 2);
	EXPECT_DOUBLE_EQ(bbox2.maxY(), 20);
	EXPECT_EQ(bbox2, BoundingBox(DPair(50, 20), DPair(5, 2)));

	BoundingBox bbox3(" 1bp    2bp  3bp 4bp  ");
	EXPECT_TRUE(bbox2.valid());
	EXPECT_FALSE(bbox2.locked());
	EXPECT_DOUBLE_EQ(bbox3.minX(), 1);
	EXPECT_DOUBLE_EQ(bbox3.minY(), 2);
	EXPECT_DOUBLE_EQ(bbox3.maxX(), 3);
	EXPECT_DOUBLE_EQ(bbox3.maxY(), 4);

	Length len(2, Length::Unit::CM);
	double bp = len.bp();
	BoundingBox bbox4(len, len, len, len);
	EXPECT_DOUBLE_EQ(bbox4.minX(), bp);
	EXPECT_DOUBLE_EQ(bbox4.minY(), bp);
	EXPECT_DOUBLE_EQ(bbox4.maxX(), bp);
	EXPECT_DOUBLE_EQ(bbox4.maxY(), bp);
}


TEST(BoundingBoxTest, extent) {
	BoundingBox bbox(5, 10, 30, 30);
	EXPECT_DOUBLE_EQ(bbox.width(), 25);
	EXPECT_DOUBLE_EQ(bbox.height(), 20);
}


TEST(BoundingBoxTest, set) {
	BoundingBox bbox;
	bbox.set("1bp 2bp 3bp 4bp");
	EXPECT_EQ(bbox, BoundingBox(1, 2, 3, 4));

	bbox.set("4bp 3bp 2bp 1bp");
	EXPECT_EQ(bbox, BoundingBox(2, 1, 4, 3));

	bbox.set("1bp");
	EXPECT_EQ(bbox, BoundingBox(1, 0, 5, 4));

	bbox.set("2bp 3bp");
	EXPECT_EQ(bbox, BoundingBox(-1, -3, 7, 7));

	EXPECT_THROW(bbox.set(""), BoundingBoxException);
	EXPECT_THROW(bbox.set("1bp 2bp 3bp"), BoundingBoxException);
}


TEST(BoundingBoxTest, embed) {
	BoundingBox bbox1(3, 4, 10, 15);
	BoundingBox bbox2(3, 4, 10, 15);
	EXPECT_EQ(bbox1, bbox2);

	bbox1.embed(6, 6);
	EXPECT_EQ(bbox1, bbox2);

	bbox1.embed(11, 14);
	EXPECT_EQ(bbox1, BoundingBox(3, 4, 11, 15));

	bbox2.embed(DPair(11, 14));
	EXPECT_EQ(bbox1, bbox2);

	bbox1.embed(Pair<int>(0, 0));
	EXPECT_EQ(bbox1, BoundingBox(0, 0, 11, 15));
	EXPECT_NE(bbox1, bbox2);

	bbox2.embed(bbox1);
	EXPECT_EQ(bbox1, bbox2);

	bbox1 = BoundingBox(5, 5, 5, 5);
	bbox1.embed(DPair(5, 5), 2);
	EXPECT_EQ(bbox1, BoundingBox(3, 3, 7, 7));

	EXPECT_NE(bbox1, bbox2);
	bbox1.invalidate();
	bbox1.embed(bbox2);
	EXPECT_EQ(bbox1, bbox2);
}


TEST(BoundingBoxTest, expand) {
	BoundingBox bbox(4, 5, 6, 7);
	bbox.expand(3);
	EXPECT_EQ(bbox, BoundingBox(1, 2, 9, 10));
}


TEST(BoundingBoxTest, lock) {
	BoundingBox bbox(4, 5, 6, 7);
	bbox.lock();
	bbox.expand(3);
	EXPECT_EQ(bbox, BoundingBox(4, 5, 6, 7));
}


TEST(BoundingBoxTest, scale) {
	BoundingBox bbox(4, 5, 6, 7);
	bbox.scale(2, 2);
	EXPECT_EQ(bbox, BoundingBox(8, 10, 12, 14));
	bbox.scale(-2, -2);
	EXPECT_EQ(bbox, BoundingBox(-24, -28, -16, -20));
}


TEST(BoundingBoxTest, inc) {
	BoundingBox bbox(4, 5, 6, 7);
	bbox += BoundingBox(8, 9, 10, 11);
	EXPECT_EQ(bbox, BoundingBox(12, 14, 16, 18));
}


TEST(BoundingBoxTest, intersect) {
	BoundingBox bbox(4, 5, 16, 17);
	bbox.intersect(BoundingBox(10, 18, 20, 25));
	EXPECT_EQ(bbox, BoundingBox(4, 5, 16, 17));

	bbox.intersect(BoundingBox(10, 10, 20, 25));
	EXPECT_EQ(bbox, BoundingBox(10, 10, 16, 17));
}


TEST(BoundingBoxTest, transform) {
	BoundingBox bbox(-5, -5, 5, 5);
	Matrix matrix(1);
	matrix.rotate(M_PI/4);
	bbox.transform(matrix);
	EXPECT_NEAR(bbox.minX(), -5.068, 0.001);
	EXPECT_NEAR(bbox.minY(), -5.068, 0.001);
	EXPECT_NEAR(bbox.maxX(),  5.068, 0.001);
	EXPECT_NEAR(bbox.maxY(),  5.068, 0.001);

	matrix.rotate(-M_PI/4).translate(2, 3);
	bbox.transform(matrix);
	EXPECT_NEAR(bbox.minX(), -3.068, 0.001);
	EXPECT_NEAR(bbox.minY(), -2.068, 0.001);
	EXPECT_NEAR(bbox.maxX(),  7.068, 0.001);
	EXPECT_NEAR(bbox.maxY(),  8.068, 0.001);
}


TEST(BoundingBoxTest, svgViewBox) {
	BoundingBox bbox(4, 5, 6, 7);
	ostringstream oss;
	oss << bbox.toSVGViewBox();
	EXPECT_EQ(oss.str(), "4 5 2 2");
}


TEST(BoundingBoxTest, svgRectElement) {
	BoundingBox bbox(4, 5, 6, 7);
	auto rect = bbox.createSVGRect();
	ASSERT_NE(rect, nullptr);
	EXPECT_EQ(rect->name(), "rect");
	EXPECT_EQ(string(rect->getAttributeValue("x")), "4");
	EXPECT_EQ(string(rect->getAttributeValue("y")), "5");
	EXPECT_EQ(string(rect->getAttributeValue("width")), "2");
	EXPECT_EQ(string(rect->getAttributeValue("height")), "2");
}


TEST(BoundingBoxTest, extractLengths) {
	vector<Length> lengths = BoundingBox::extractLengths(" 1cm,2mm  , 3pt   5in");
	ASSERT_EQ(lengths.size(), 4u);
	EXPECT_DOUBLE_EQ(lengths[0].cm(), 1);
	EXPECT_DOUBLE_EQ(lengths[1].mm(), 2);
	EXPECT_DOUBLE_EQ(lengths[2].pt(), 3);
	EXPECT_DOUBLE_EQ(lengths[3].in(), 5);
}
