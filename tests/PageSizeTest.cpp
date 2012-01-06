/*************************************************************************
** PageSizeTest.cpp                                                     **
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
#include <limits>
#include "PageSize.h"


class PageSizeTest : public ::testing::Test
{
	protected:
		struct PageData 
		{
			const char *id;
			double width, height;
		};

		static const PageData pageData[];
		
		PageSize ps;
};


const PageSizeTest::PageData PageSizeTest::pageData[] = {
	{"A4", 210, 297},
	{"a4", 210, 297}, 
	{"a4-p", 210, 297}, 
	{"a4-portrait", 210, 297}, 
	{"a4-l", 297, 210}, 
	{"a4-landscape", 297, 210}, 
	{"a5", 148, 210}, 
	{"c10", 28, 40},
	{"letter", 216, 279},
	{0, 0, 0}
};


TEST_F(PageSizeTest, resize) {
	EXPECT_FALSE(ps.valid());
	
	for (const PageData *p = pageData; p && p->id; p++) {
		ps.resize(p->id);
		EXPECT_DOUBLE_EQ(ps.widthInMM(), p->width);
		EXPECT_DOUBLE_EQ(ps.heightInMM(), p->height);
	}
}


TEST_F(PageSizeTest, exceptions) {
	EXPECT_THROW(ps.resize("a"), PageSizeException);
	EXPECT_THROW(ps.resize("e4"), PageSizeException);
	EXPECT_THROW(ps.resize("a4-unknown"), PageSizeException);
}



