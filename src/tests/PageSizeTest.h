/***********************************************************************
** PageSizeTest.h                                                     **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/
// $Id: PageSizeTest.h,v 1.1 2006/01/06 13:32:41 mgieseki Exp $

#include <cxxtest/TestSuite.h>
#include <limits>
#include "PageSize.h"


class PageSizeTest : public CxxTest::TestSuite
{
	struct PageData 
	{
		const char *id;
		double width, height;
	};
	
	public:
		void test_resize () {
			TS_ASSERT(!ps.valid());
			
			for (const PageData *p = pageData; p && p->id; p++) {
				ps.resize(p->id);
				TS_ASSERT_DELTA(ps.widthInMM(), p->width, 0.0000001);
				TS_ASSERT_DELTA(ps.heightInMM(), p->height, 0.000001);
			}
		}

		void test_exceptions () {
			TS_ASSERT_THROWS(ps.resize("a"), PageSizeException);
			TS_ASSERT_THROWS(ps.resize("e4"), PageSizeException);
			TS_ASSERT_THROWS(ps.resize("a4-unknown"), PageSizeException);
		}
	private:
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
