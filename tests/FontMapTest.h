/***********************************************************************
** FontMapTest.h                                                      **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#include <cxxtest/TestSuite.h>
#include <fstream>
#include "FontMap.h"

using std::ifstream;

class FontMapTest : public CxxTest::TestSuite
{
	public:
		void test_pdf_map () {
			const char *fname = "tests/dvipdfm_test.map";
			ifstream ifs(fname);
			if (ifs) {
				FontMap fm(fname);
				TS_ASSERT_EQUALS(strcmp(fm.lookup("MyriadPro-Bold-8t"), "MyriadPro-Bold"), 0);
				TS_ASSERT_EQUALS(strcmp(fm.lookup("cmbsy8"), "cmbsy7"), 0);
				// entry without mapped name
				TS_ASSERT_EQUALS(fm.lookup("msam10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("msbm10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("msbm10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("i don't exist"), (const char*)0);

				TS_ASSERT_EQUALS(strcmp(fm.encoding("MyriadPro-Bold-8t"), "my-lf-t1"), 0);
				TS_ASSERT_EQUALS(fm.encoding("cmbsy8"), (const char*)0);
			}
		}


		void test_ps_map () {
			const char *fname = "tests/dvips_test.map";
			ifstream ifs(fname);
			if (ifs) {
				FontMap fm(fname);
				TS_ASSERT_EQUALS(strcmp(fm.lookup("MyriadPro-Bold-8t"), "MyriadPro-Bold"), 0);
				TS_ASSERT_EQUALS(strcmp(fm.lookup("cmbsy8"), "cmbsy7"), 0);
				// entry without mapped name
				TS_ASSERT_EQUALS(fm.lookup("msam10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("msbm10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("msbm10"), (const char*)0);
				TS_ASSERT_EQUALS(fm.lookup("i don't exist"), (const char*)0);

				TS_ASSERT_EQUALS(strcmp(fm.encoding("MyriadPro-Bold-8t"), "my-lf-t1"), 0);
				TS_ASSERT_EQUALS(fm.encoding("cmbsy8"), (const char*)0);

			}
		}
};
