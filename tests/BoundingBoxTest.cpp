/*************************************************************************
** BoundingBoxTest.cpp                                                  **
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
#include "BoundingBox.h"

TEST(BoundingBoxTest, set) {
	BoundingBox bbox;
	bbox.set("1pt 2pt 3pt 4pt");
	EXPECT_EQ(bbox.minX(), 1);
	EXPECT_EQ(bbox.minY(), 2);
	EXPECT_EQ(bbox.maxX(), 3);
	EXPECT_EQ(bbox.maxY(), 4);

	bbox.set("4pt 3pt 2pt 1pt");
	EXPECT_EQ(bbox.minX(), 2);
	EXPECT_EQ(bbox.minY(), 1);
	EXPECT_EQ(bbox.maxX(), 4);
	EXPECT_EQ(bbox.maxY(), 3);

	bbox.set("1pt");
	EXPECT_EQ(bbox.minX(), 1);
	EXPECT_EQ(bbox.minY(), 0);
	EXPECT_EQ(bbox.maxX(), 5);
	EXPECT_EQ(bbox.maxY(), 4);

	bbox.set("2pt 3pt");
	EXPECT_EQ(bbox.minX(), -1);
	EXPECT_EQ(bbox.minY(), -3);
	EXPECT_EQ(bbox.maxX(), 7);
	EXPECT_EQ(bbox.maxY(), 7);

	EXPECT_THROW(bbox.set(""), BoundingBoxException);
	EXPECT_THROW(bbox.set("1pt 2pt 3pt"), BoundingBoxException);
}

