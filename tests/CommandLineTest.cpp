/*************************************************************************
** CommandLineTest.cpp                                                  **
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
#include <cstring>
#include "CommandLine.h"

TEST(CommandLineTest, noarg_short) {
	CommandLine cmd;
	const char *args[] = {"progname", "-s", "-n", "-h"};
	char **argv = const_cast<char**>(args);
	cmd.parse(4, argv, false);

	EXPECT_TRUE(cmd.stdout_given());
	EXPECT_TRUE(cmd.no_fonts_given());
	EXPECT_TRUE(cmd.help_given());
	EXPECT_FALSE(cmd.list_specials_given());
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);
}


TEST(CommandLineTest, noarg_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--no-fonts", "--stdout", "--help"};
	char **argv = const_cast<char**>(args);
	cmd.parse(4, argv, false);

	EXPECT_TRUE(cmd.stdout_given());
	EXPECT_TRUE(cmd.no_fonts_given());
	EXPECT_TRUE(cmd.help_given());
	EXPECT_FALSE(cmd.list_specials_given());
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);
}


TEST(CommandLineTest, arg_short) {
	CommandLine cmd;
	const char *args[] = {"progname", "-p5", "-r45", "-omyfile.xyz", "-ayes"};
	char **argv = const_cast<char**>(args);
	cmd.parse(5, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "5");
	EXPECT_TRUE(cmd.rotate_given());
	EXPECT_EQ(cmd.rotate_arg(), 45);
	EXPECT_TRUE(cmd.output_given());
	EXPECT_EQ(cmd.output_arg(), "myfile.xyz");
	EXPECT_FALSE(cmd.bbox_given());
	EXPECT_EQ(cmd.bbox_arg(), "min");
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);
	EXPECT_TRUE(cmd.trace_all_given());
	EXPECT_TRUE(cmd.trace_all_arg());
}


TEST(CommandLineTest, arg_combined) {
	CommandLine cmd;
	const char *args[] = {"progname", "-lse", "-p5", "-omyfile.xyz"};
	char **argv = const_cast<char**>(args);
	cmd.parse(4, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "5");
	EXPECT_TRUE(cmd.list_specials_given());
	EXPECT_TRUE(cmd.stdout_given());
	EXPECT_TRUE(cmd.exact_given());
	EXPECT_FALSE(cmd.error());
}


TEST(CommandLineTest, arg_separated) {
	CommandLine cmd;
	const char *args[] = {"progname", "-p", "5", "-r",  "45", "myfile.xyz", "-afalse"};
	char **argv = const_cast<char**>(args);
	cmd.parse(7, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "5");
	EXPECT_TRUE(cmd.rotate_given());
	EXPECT_EQ(cmd.rotate_arg(), 45);
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 1);
	EXPECT_TRUE(cmd.trace_all_given());
	EXPECT_FALSE(cmd.trace_all_arg());
}


TEST(CommandLineTest, arg_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--page=9", "--rotate=-45.5", "--trace-all", "--output=myfile.zyx"};
	char **argv = const_cast<char**>(args);
	cmd.parse(5, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "9");
	EXPECT_TRUE(cmd.rotate_given());
	EXPECT_EQ(cmd.rotate_arg(), -45.5);
	EXPECT_TRUE(cmd.output_given());
	EXPECT_EQ(cmd.output_arg(), "myfile.zyx");
	EXPECT_FALSE(cmd.bbox_given());
	EXPECT_EQ(cmd.bbox_arg(), "min");
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);
	EXPECT_TRUE(cmd.trace_all_given());
	EXPECT_FALSE(cmd.trace_all_arg());
}


TEST(CommandLineTest, abbrev_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--pa=9", "--rot=-45.5", "--out=myfile.zyx"};
	char **argv = const_cast<char**>(args);
	cmd.parse(4, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "9");
	EXPECT_TRUE(cmd.rotate_given());
	EXPECT_EQ(cmd.rotate_arg(), -45.5);
	EXPECT_TRUE(cmd.output_given());
	EXPECT_EQ(cmd.output_arg(), "myfile.zyx");
	EXPECT_FALSE(cmd.bbox_given());
	EXPECT_EQ(cmd.bbox_arg(), "min");
	EXPECT_FALSE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);
}


TEST(CommandLineTest, error) {
	CommandLine cmd;
	// error on wrong argument type
	const char *args1[] = {"progname", "--rotate=x"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(2, argv, false);

	EXPECT_FALSE(cmd.rotate_given());
	EXPECT_TRUE(cmd.error());

	// error on missing arguments
	const char *args2[] = {"progname", "--page", "--zip"};
	argv = const_cast<char**>(args2);
	cmd.parse(3, argv, false);
	EXPECT_TRUE(cmd.zip_given());
	EXPECT_EQ(cmd.zip_arg(), 9);
	EXPECT_FALSE(cmd.page_given());
	EXPECT_FALSE(cmd.rotate_given());
	EXPECT_TRUE(cmd.error());
	EXPECT_EQ(cmd.numFiles(), 0);

	// error on ambiguous abbreviation of long option
	const char *args3[] = {"progname", "--no"};
	argv = const_cast<char**>(args3);
	cmd.parse(2, argv, false);
	EXPECT_TRUE(cmd.error());

	// incorrect boolean value
	const char *args4[] = {"progname", "--trace-all=nope"};
	argv = const_cast<char**>(args4);
	cmd.parse(2, argv, false);
	EXPECT_FALSE(cmd.trace_all_given());
	EXPECT_TRUE(cmd.error());
}


TEST(CommandLineTest, file) {
	CommandLine cmd;
	const char *args1[] = {"progname", "--page=3", "-z5", "myfile1", "-l", "myfile2"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(6, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "3");
	EXPECT_TRUE(cmd.zip_given());
	EXPECT_EQ(cmd.zip_arg(), 5);
	EXPECT_TRUE(cmd.list_specials_given());
	EXPECT_EQ(cmd.numFiles(), 2);
	EXPECT_EQ(strcmp(cmd.file(0), "myfile1"), 0);
	EXPECT_EQ(strcmp(cmd.file(1), "myfile2"), 0);
	EXPECT_FALSE(cmd.error());
}


TEST(CommandLineTest, files_only) {
	CommandLine cmd;
	const char *args1[] = {"progname", "--page=3", "-z5", "--", "-l", "myfile"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(6, argv, false);

	EXPECT_TRUE(cmd.page_given());
	EXPECT_EQ(cmd.page_arg(), "3");
	EXPECT_TRUE(cmd.zip_given());
	EXPECT_EQ(cmd.zip_arg(), 5);
	EXPECT_EQ(cmd.numFiles(), 2);
	EXPECT_EQ(strcmp(cmd.file(0), "-l"), 0);
	EXPECT_EQ(strcmp(cmd.file(1), "myfile"), 0);
}

