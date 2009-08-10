/***********************************************************************
** StreamInputBufferTest.h                                            **
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
#include <sstream>
#include "InputBuffer.h"
#include "InputReader.h"

using std::istringstream;

class StreamInputBufferTest : public CxxTest::TestSuite
{
	public:
		void test_get () {
			istringstream iss("abcdefghijklmnopqrstuvwxyz"); 
			StreamInputBuffer buffer(iss, 10);
			BufferInputReader in(buffer);
			bool ok=true;
			for (int i=0; !in.eof() && ok; i++) {
				TS_ASSERT_LESS_THAN(i, 26);
				TS_ASSERT_EQUALS(in.get(), 'a'+i);
				ok = (i < 26);
			}
		}

		void test_peek () {
			istringstream iss("abcdefghijklmnopqrstuvwxyz"); 
			StreamInputBuffer buffer(iss, 10);
			BufferInputReader in(buffer);
			TS_ASSERT_EQUALS(in.peek(), 'a');
			for (int i=0; i < 20; i++) 
				TS_ASSERT_EQUALS(in.peek(i), 'a'+i);
			// we can't look forward more than BUFSIZE characters (10 in this case)
			for (int i=21; i < 26; i++) 
				TS_ASSERT_EQUALS(in.peek(i), -1);
		}

		void test_check () {
			istringstream iss("abcdefghijklmnopqrstuvwxyz"); 
			StreamInputBuffer buffer(iss, 10);
			BufferInputReader in(buffer);
			TS_ASSERT(in.check("abc", false));
			TS_ASSERT(in.check("abc", true));
			TS_ASSERT(in.check("def", true));
			TS_ASSERT(in.check("ghi", true));
			TS_ASSERT(in.check("jkl", true));
			TS_ASSERT(in.check("mnopqrst", false));
			TS_ASSERT(in.check("mnopqrst", true));
			TS_ASSERT(in.check("uvwxyz", true));
		}

		void test_skip () {
			istringstream iss("abcdefghijklmnopqrstuvwxyz"); 
			StreamInputBuffer buffer(iss, 10);
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
			istringstream iss("1234,-5,+6,10.-"); 
			StreamInputBuffer buffer(iss, 10);
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
			istringstream iss("1234,-5,10,1abc,1234a"); 
			StreamInputBuffer buffer(iss, 10);
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
			istringstream iss("1234,-5,6.12,-3.1415,-.1,12e2,10.-"); 
			StreamInputBuffer buffer(iss, 10);
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


		void test_attribs () {
			istringstream iss("aaa=1 bbb=2 ccc=3 d e"); 
			StreamInputBuffer buffer(iss, 10);
			BufferInputReader in(buffer);
			map<string,string> attr;
			int s = in.parseAttributes(attr);
			TS_ASSERT_EQUALS(s, 3);
			TS_ASSERT_EQUALS(attr["aaa"], "1");
			TS_ASSERT_EQUALS(attr["bbb"], "2");
			TS_ASSERT_EQUALS(attr["ccc"], "3");
		}
};
