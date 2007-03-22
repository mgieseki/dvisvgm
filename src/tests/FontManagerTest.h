/***********************************************************************
** FontManagerTest.h                                                  **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id$

#include <cxxtest/TestSuite.h>
#include <sstream>
#include "Font.h"
#include "FontManager.h"

class FontManagerTest : public CxxTest::TestSuite
{
	public:
		FontManagerTest () {
			fm.registerFont(10, "cmr10", 1274110073, 10, 10);
			fm.registerFont(11, "cmr10", 1274110073, 20, 10);
			fm.registerFont( 9, "cmr10", 1274110073, 30, 10);
		}


		void test_fontID () {
			TS_ASSERT_EQUALS(fm.fontID(10), 0);
			TS_ASSERT_EQUALS(fm.fontID(11), 1);
			TS_ASSERT_EQUALS(fm.fontID(9), 2);
			TS_ASSERT_EQUALS(fm.fontID(1), -1);
		}

		void test_getFont () {
			const Font *f1 = fm.getFont(10);
			TS_ASSERT(f1);
			TS_ASSERT_EQUALS(f1->name(), "cmr10");
			TS_ASSERT(dynamic_cast<const PhysicalFontImpl*>(f1));
			
			const Font *f2 = fm.getFont(11);
			TS_ASSERT(f2);
			TS_ASSERT_DIFFERS(f1, f2);
			TS_ASSERT_EQUALS(f2->name(), "cmr10");
			TS_ASSERT(dynamic_cast<const PhysicalFontProxy*>(f2));
		}

	private:
		FontManager fm;
};
