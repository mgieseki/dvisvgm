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
#include "FontMap.h"

using std::istringstream;

class FontMapTest : public CxxTest::TestSuite
{
	public:
		void test_split () {
			string str = 
//				"cork-anttb cork-antt anttb\n"
//				"cork-anttbcap cork-anttcap anttb\n"
//				"cmr10 ot1 -r\n"
//				"ptmro8r 8r Times-Roman -s 0.167\n"
				"pncbo8r 8r pncb8a -s 0.167\n";
			istringstream iss(str);
			fm.clear();
			fm.read(iss);			
			TS_ASSERT_EQUALS();
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
			TS_ASSERT(f1 != f2);
			TS_ASSERT_EQUALS(f2->name(), "cmr10");
			TS_ASSERT(dynamic_cast<const PhysicalFontProxy*>(f2));
		}

	private:
		FontMap *fm;
};
