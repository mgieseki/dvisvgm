/*************************************************************************
** CommandLineTest.h                                                    **
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
#include "CommandLine.h"

class CommandLineTest : public CxxTest::TestSuite
{
	public:
		void test_noarg_short () {
			const char *args[] = {"progname", "-s", "-n", "-h"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.stdout_given());
			TS_ASSERT(cmd.no_fonts_given());
			TS_ASSERT(cmd.help_given());
			TS_ASSERT(!cmd.list_specials_given());
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);
		}		

		void test_noarg_long () {
			const char *args[] = {"progname", "--no-fonts", "--stdout", "--help"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.stdout_given());
			TS_ASSERT(cmd.no_fonts_given());
			TS_ASSERT(cmd.help_given());
			TS_ASSERT(!cmd.list_specials_given());
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);
		}

		void test_arg_short () {
			const char *args[] = {"progname", "-p5", "-r45", "-omyfile.xyz"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 5);
			TS_ASSERT(cmd.rotate_given());
			TS_ASSERT_EQUALS(cmd.rotate_arg(), 45);
			TS_ASSERT(cmd.output_given());
			TS_ASSERT_EQUALS(cmd.output_arg(), "myfile.xyz");
			TS_ASSERT(!cmd.bbox_given());
			TS_ASSERT_EQUALS(cmd.bbox_arg(), "min");
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);
		}

		void test_arg_combined () {
			const char *args[] = {"progname", "-lsn", "-p5", "-omyfile.xyz"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 5);
			TS_ASSERT(cmd.list_specials_given());
			TS_ASSERT(cmd.stdout_given());
			TS_ASSERT(cmd.no_fonts_given());
			TS_ASSERT(!cmd.error());
		}

		void test_arg_separated () {
			const char *args[] = {"progname", "-p", "5", "-r",  "45", "myfile.xyz"};
			char **argv = const_cast<char**>(args);
			cmd.parse(6, argv, false);

			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 5);
			TS_ASSERT(cmd.rotate_given());
			TS_ASSERT_EQUALS(cmd.rotate_arg(), 45);
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 1);
		}

		void test_arg_long () {
			const char *args[] = {"progname", "--page=9", "--rotate=-45.5", "--output=myfile.zyx"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 9);
			TS_ASSERT(cmd.rotate_given());
			TS_ASSERT_EQUALS(cmd.rotate_arg(), -45.5);
			TS_ASSERT(cmd.output_given());
			TS_ASSERT_EQUALS(cmd.output_arg(), "myfile.zyx");
			TS_ASSERT(!cmd.bbox_given());
			TS_ASSERT_EQUALS(cmd.bbox_arg(), "min");
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);
		}

		void test_abbrev_long () {
			const char *args[] = {"progname", "--pa=9", "--rot=-45.5", "--out=myfile.zyx"};
			char **argv = const_cast<char**>(args);
			cmd.parse(4, argv, false);

			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 9);
			TS_ASSERT(cmd.rotate_given());
			TS_ASSERT_EQUALS(cmd.rotate_arg(), -45.5);
			TS_ASSERT(cmd.output_given());
			TS_ASSERT_EQUALS(cmd.output_arg(), "myfile.zyx");
			TS_ASSERT(!cmd.bbox_given());
			TS_ASSERT_EQUALS(cmd.bbox_arg(), "min");
			TS_ASSERT(!cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);
		}

		void test_error () {
			// error on wrong argument type
			const char *args1[] = {"progname", "--page=x"};
			char **argv = const_cast<char**>(args1);
			cmd.parse(2, argv, false);

			TS_ASSERT(!cmd.page_given());
			TS_ASSERT(cmd.error());

			// error on missing arguments
			const char *args2[] = {"progname", "--page", "--zip"};
			argv = const_cast<char**>(args2);
			cmd.parse(3, argv, false);
			TS_ASSERT(cmd.zip_given());
			TS_ASSERT_EQUALS(cmd.zip_arg(), 9);
			TS_ASSERT(!cmd.page_given());
			TS_ASSERT(!cmd.rotate_given());
			TS_ASSERT(cmd.error());
			TS_ASSERT_EQUALS(cmd.numFiles(), 0);

			// error on ambiguous abbreviation of long option
			const char *args3[] = {"progname", "--no"};
			argv = const_cast<char**>(args3);
			cmd.parse(2, argv, false);
			TS_ASSERT(cmd.error());
		}

		void test_file () {
			const char *args1[] = {"progname", "--page=3", "-z5", "myfile1", "-l", "myfile2"};
			char **argv = const_cast<char**>(args1);
			cmd.parse(6, argv, false);
		
			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 3);
			TS_ASSERT(cmd.zip_given());
			TS_ASSERT_EQUALS(cmd.zip_arg(), 5);
			TS_ASSERT(cmd.list_specials_given());
			TS_ASSERT_EQUALS(cmd.numFiles(), 2);
			TS_ASSERT_EQUALS(strcmp(cmd.file(0), "myfile1"), 0);
			TS_ASSERT_EQUALS(strcmp(cmd.file(1), "myfile2"), 0);
			TS_ASSERT(!cmd.error());
		}

		void test_files_only () {
			const char *args1[] = {"progname", "--page=3", "-z5", "--", "-l", "myfile"};
			char **argv = const_cast<char**>(args1);
			cmd.parse(6, argv, false);
		
			TS_ASSERT(cmd.page_given());
			TS_ASSERT_EQUALS(cmd.page_arg(), 3);
			TS_ASSERT(cmd.zip_given());
			TS_ASSERT_EQUALS(cmd.zip_arg(), 5);
			TS_ASSERT_EQUALS(cmd.numFiles(), 2);
			TS_ASSERT_EQUALS(strcmp(cmd.file(0), "-l"), 0);
			TS_ASSERT_EQUALS(strcmp(cmd.file(1), "myfile"), 0);
		}

	private:
		CommandLine cmd;
};
