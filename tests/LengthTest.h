/*************************************************************************
** LengthTest.h                                                         **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <cxxtest/TestSuite.h>
#include "Length.h"

class LengthTest : public CxxTest::TestSuite
{
	public:
		void test_set1 () {
			Length len;
			len.set(1, Length::PT);
			TS_ASSERT_EQUALS(len.pt(), 1);
			
			len.set(72, Length::BP);
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.bp(), 72);
			TS_ASSERT_EQUALS(len.in(), 1);

			len.set(1, Length::IN);
			TS_ASSERT_EQUALS(len.pt(), 72.27);

			len.set(12, Length::PC);
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.in(), 1);

			len.set(1, Length::CM);
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);

			len.set(10, Length::MM);
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);
		}


		void test_set2 () {
			Length len;
			len.set(1, "pt");
			TS_ASSERT_EQUALS(len.pt(), 1);
			
			len.set(72, "bp");
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.bp(), 72);
			TS_ASSERT_EQUALS(len.in(), 1);
			TS_ASSERT_EQUALS(len.pc(), 12);

			len.set(1, "in");
			TS_ASSERT_EQUALS(len.pt(), 72.27);

			len.set(12, "pc");
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.in(), 1);
			TS_ASSERT_EQUALS(len.pc(), 12);

			len.set(1, "cm");
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);

			len.set(10, "mm");
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);

			len.set(1, "");
			TS_ASSERT_EQUALS(len.pt(), 1);

			TS_ASSERT_THROWS(len.set(1, "xy"), UnitException)
		}

		void test_set3 () {
			Length len;
			len.set("1pt");
			TS_ASSERT_EQUALS(len.pt(), 1);
			
			len.set("72bp");
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.bp(), 72);
			TS_ASSERT_EQUALS(len.in(), 1);
			TS_ASSERT_EQUALS(len.pc(), 12);

			len.set("1in");
			TS_ASSERT_EQUALS(len.pt(), 72.27);

			len.set("12pc");
			TS_ASSERT_EQUALS(len.pt(), 72.27);
			TS_ASSERT_EQUALS(len.in(), 1);
			TS_ASSERT_EQUALS(len.pc(), 12);

			len.set("1cm");
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);

			len.set("10mm");
			TS_ASSERT_EQUALS(len.pt(), 2.54*72.27);
			TS_ASSERT_EQUALS(len.cm(), 1);
			TS_ASSERT_EQUALS(len.mm(), 10);

			len.set("1");
			TS_ASSERT_EQUALS(len.pt(), 1);

			TS_ASSERT_THROWS(len.set("1xy"), UnitException)
			TS_ASSERT_THROWS(len.set("pt"), UnitException)
		}

};
