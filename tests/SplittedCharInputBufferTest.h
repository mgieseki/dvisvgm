/*************************************************************************
** SplittedCharInputBufferTest.h                                        **
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
#include <cstring>
#include <sstream>
#include "InputBuffer.h"
#include "InputReader.h"

using std::istringstream;

class SplittedCharInputBufferTest : public CxxTest::TestSuite
{
	public:
		void test_get () {
			const char *buf1 = "abcdef";
			const char *buf2 = "ghijklmnopqrstuvwxyz";
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			bool ok=true;
			for (int i=0; !in.eof() && ok; i++) {
				TS_ASSERT_LESS_THAN(i, 26);
				TS_ASSERT_EQUALS(in.get(), 'a'+i);
				ok = (i < 26);
			}
		}

		void test_peek () {
			const char *buf1 = "abcdef";
			const char *buf2 = "ghijklmnopqrstuvwxyz";
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			TS_ASSERT_EQUALS(in.peek(), 'a');
			for (int i=0; i < 26; i++) 
				TS_ASSERT_EQUALS(in.peek(i), 'a'+i);
			TS_ASSERT_EQUALS(in.peek(26), -1);
		}

		void test_check () {
			const char *buf1 = "abcde";
			const char *buf2 = "fghijklmnopqrstuvwxyz";
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			TS_ASSERT(in.check("abc", false));
			TS_ASSERT(in.check("abc", true));
			TS_ASSERT(in.check("def", true));
			TS_ASSERT(in.check("ghi", true));
			TS_ASSERT(in.check("jkl", true));
			TS_ASSERT(in.check("mnopqrst", false));
			TS_ASSERT(in.check("mnopqrst", true));
			TS_ASSERT(in.check("uvwxyz", true));
			TS_ASSERT_EQUALS(in.get(), -1);
		}

		void test_skip () {
			const char *buf1 = "abcde";
			const char *buf2 = "fghijklmnopqrstuvwxyz";
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			in.skip(3);
			TS_ASSERT_EQUALS(in.peek(), 'd');
			in.skipUntil("ijk", false);
			TS_ASSERT_EQUALS(in.peek(), 'i');
			in.skipUntil("ijk", true);
			TS_ASSERT_EQUALS(in.peek(), 'l');
			in.skipUntil("z", true);
			TS_ASSERT(in.eof());
		}

		void test_parseInt () {
			const char *buf1 = "1234,-"; 
			const char *buf2 = "5,+6,10.-"; 
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			int n;
			TS_ASSERT(in.parseInt(n));
			TS_ASSERT_EQUALS(n, 1234);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(n));
			TS_ASSERT_EQUALS(n, -5);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(n));
			TS_ASSERT_EQUALS(n, 6);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(n));
			TS_ASSERT_EQUALS(n, 10);
			TS_ASSERT_EQUALS(in.get(), '.');

			TS_ASSERT(!in.parseInt(n));
			TS_ASSERT_EQUALS(in.get(), '-');
		}

		void test_parseInt_base () {
			const char *buf1 = "1234,-5,10,1a"; 
			const char *buf2 = "bc,1234a"; 
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			int n;
			TS_ASSERT(in.parseInt(10, n));
			TS_ASSERT_EQUALS(n, 1234);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(!in.parseInt(10, n));
			in.get();
			TS_ASSERT(in.parseInt(10, n));
			TS_ASSERT_EQUALS(n, 5);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(16, n));
			TS_ASSERT_EQUALS(n, 16);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(16, n));
			TS_ASSERT_EQUALS(n, 0x1abc);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT(in.parseInt(8, n));
			TS_ASSERT_EQUALS(n, 01234);
			TS_ASSERT_EQUALS(in.get(), 'a');
		}

		void test_parseDouble () {
			const char *buf1 = "1234,-5,6.12,-3"; 
			const char *buf2 = ".1415,-.1,12e2,10.-"; 
			SplittedCharInputBuffer buffer(buf1, strlen(buf1), buf2, strlen(buf2));
			BufferInputReader in(buffer);
			double d;
			TS_ASSERT_EQUALS(in.parseDouble(d), 'i');
			TS_ASSERT_EQUALS(d, 1234.0);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'i');
			TS_ASSERT_EQUALS(d, -5.0);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'f');
			TS_ASSERT_EQUALS(d, 6.12);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'f');
			TS_ASSERT_EQUALS(d, -3.1415);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'f');
			TS_ASSERT_EQUALS(d, -0.1);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'f');
			TS_ASSERT_EQUALS(d, 1200);
			TS_ASSERT_EQUALS(in.get(), ',');

			TS_ASSERT_EQUALS(in.parseDouble(d), 'f');
			TS_ASSERT_EQUALS(d, 10.0);
			TS_ASSERT_EQUALS(in.peek(), '-');

			TS_ASSERT(!in.parseDouble(d));
			TS_ASSERT_EQUALS(in.get(), '-');
		}
};
