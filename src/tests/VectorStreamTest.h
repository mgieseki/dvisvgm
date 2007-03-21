/***********************************************************************
** VectorStreamTest.h                                                 **
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
#include "VectorStream.h"

using std::string;

class VectorStreamTest : public CxxTest::TestSuite
{
	public:
		void test_read () {
			const char *str = "abcdefghijklm\0nopqrstuvwxyz";
			vector<char> vec(str, str+27);
			VectorInputStream<char> vs(vec);
			for (unsigned count = 0; vs; count++) {
				int c = vs.get();
				if (count < vec.size()) {
					TS_ASSERT_EQUALS(c, str[count]);
				}
				else {
					TS_ASSERT_EQUALS(c, -1);
				}
			}
		}
};
