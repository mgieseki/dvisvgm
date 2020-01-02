/*************************************************************************
** PageSizeTest.cpp                                                     **
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
#include "PageSize.hpp"

using namespace std;


struct PageData {
	const char *name;
	double width, height;  // in mm
};


const vector<PageData> pageData = {
	{"A4", 210, 297},
	{"a4", 210, 297},
	{"a4-p", 210, 297},
	{"a4-portrait", 210, 297},
	{"a4-l", 297, 210},
	{"a4-landscape", 297, 210},
	{"a5", 148, 210},
	{"b2", 500, 707},
	{"c10", 28, 40},
	{"D3", 272, 385},
	{"letter", 216, 279}
};


TEST(PageSizeTest, resize) {
	PageSize pageSize;
	EXPECT_FALSE(pageSize.valid());
	for (const PageData &data : pageData) {
		pageSize.resize(data.name);
		EXPECT_DOUBLE_EQ(pageSize.width().mm(), data.width) << data.name;
		EXPECT_DOUBLE_EQ(pageSize.height().mm(), data.height) << data.name;
	}
}


TEST(PageSizeTest, exceptions) {
	PageSize pageSize;
	EXPECT_THROW(pageSize.resize("a"), PageSizeException);
	EXPECT_THROW(pageSize.resize("e4"), PageSizeException);
	EXPECT_THROW(pageSize.resize("a4-unknown"), PageSizeException);
}
