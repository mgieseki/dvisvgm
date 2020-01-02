/*************************************************************************
** CommandLineTest.cpp                                                  **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <sstream>
#include "CommandLine.hpp"

using namespace std;

TEST(CommandLineTest, noarg_short) {
	CommandLine cmd;
	const char *args[] = {"progname", "-s", "-n", "-h"};
	char **argv = const_cast<char**>(args);
	cmd.parse(4, argv);

	EXPECT_TRUE(cmd.stdoutOpt.given());
	EXPECT_TRUE(cmd.noFontsOpt.given());
	EXPECT_TRUE(cmd.helpOpt.given());
	EXPECT_FALSE(cmd.listSpecialsOpt.given());
	EXPECT_FALSE(cmd.verbosityOpt.given());
	EXPECT_EQ(cmd.verbosityOpt.value(), 7u);
	EXPECT_EQ(cmd.filenames().size(), 0u);
}


TEST(CommandLineTest, noarg_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--progress", "--no-fonts", "--stdout", "--help", "--verbosity=5"};
	char **argv = const_cast<char**>(args);
	cmd.parse(6, argv);

	EXPECT_TRUE(cmd.progressOpt.given());
	EXPECT_DOUBLE_EQ(cmd.progressOpt.value(), 0.5);
	EXPECT_TRUE(cmd.stdoutOpt.given());
	EXPECT_TRUE(cmd.noFontsOpt.given());
	EXPECT_TRUE(cmd.helpOpt.given());
	EXPECT_FALSE(cmd.listSpecialsOpt.given());
	EXPECT_FALSE(cmd.stdinOpt.given());
	EXPECT_FALSE(cmd.singleDashGiven());
	EXPECT_EQ(cmd.filenames().size(), 0u);
	EXPECT_TRUE(cmd.verbosityOpt.given());
	EXPECT_EQ(cmd.verbosityOpt.value(), 5u);
}


TEST(CommandLineTest, arg_short) {
	CommandLine cmd;
	const char *args[] = {"progname", "-P", "-p5", "-r45", "-omyfile.xyz", "-ayes", "-v3", "-"};
	char **argv = const_cast<char**>(args);
	cmd.parse(8, argv);

	EXPECT_TRUE(cmd.pdfOpt.given());
	EXPECT_DOUBLE_EQ(cmd.progressOpt.value(), 0.5);
	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "5");
	EXPECT_TRUE(cmd.rotateOpt.given());
	EXPECT_EQ(cmd.rotateOpt.value(), 45);
	EXPECT_TRUE(cmd.outputOpt.given());
	EXPECT_EQ(cmd.outputOpt.value(), "myfile.xyz");
	EXPECT_FALSE(cmd.bboxOpt.given());
	EXPECT_FALSE(cmd.stdinOpt.given());
	EXPECT_EQ(cmd.bboxOpt.value(), "min");
	EXPECT_EQ(cmd.filenames().size(), 0u);
	EXPECT_TRUE(cmd.traceAllOpt.given());
	EXPECT_TRUE(cmd.traceAllOpt.value());
	EXPECT_TRUE(cmd.verbosityOpt.given());
	EXPECT_TRUE(cmd.singleDashGiven());
	EXPECT_EQ(cmd.verbosityOpt.value(), 3u);
}


TEST(CommandLineTest, arg_combined) {
	CommandLine cmd;
	const char *args1[] = {"progname", "-lse", "-p5", "-omyfile.xyz"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(4, argv);
	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "5");
	EXPECT_TRUE(cmd.listSpecialsOpt.given());
	EXPECT_TRUE(cmd.stdoutOpt.given());
	EXPECT_TRUE(cmd.exactBboxOpt.given());

	// can't combine short options that accept optional parameters
	const char *args2[] = {"progname", "-nls", "-p5", "-omyfile.xyz"};
	argv = const_cast<char**>(args2);
	EXPECT_THROW(cmd.parse(4, argv), CL::CommandLineException);
}


TEST(CommandLineTest, arg_separated) {
	CommandLine cmd;
	const char *args[] = {"progname", "-p", "5", "-r",  "45", "myfile.xyz", "-afalse"};
	char **argv = const_cast<char**>(args);
	cmd.parse(7, argv);

	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "5");
	EXPECT_TRUE(cmd.rotateOpt.given());
	EXPECT_EQ(cmd.rotateOpt.value(), 45);
	EXPECT_EQ(cmd.filenames().size(), 1u);
	EXPECT_TRUE(cmd.traceAllOpt.given());
	EXPECT_FALSE(cmd.traceAllOpt.value());
}


TEST(CommandLineTest, arg_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--page=9", "--rotate=-45.5", "--trace-all", "--output=myfile.zyx"};
	char **argv = const_cast<char**>(args);
	cmd.parse(5, argv);

	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "9");
	EXPECT_TRUE(cmd.rotateOpt.given());
	EXPECT_EQ(cmd.rotateOpt.value(), -45.5);
	EXPECT_TRUE(cmd.outputOpt.given());
	EXPECT_EQ(cmd.outputOpt.value(), "myfile.zyx");
	EXPECT_FALSE(cmd.bboxOpt.given());
	EXPECT_EQ(cmd.bboxOpt.value(), "min");
	EXPECT_EQ(cmd.filenames().size(), 0u);
	EXPECT_TRUE(cmd.traceAllOpt.given());
	EXPECT_FALSE(cmd.traceAllOpt.value());
}


TEST(CommandLineTest, string_arg) {
	CommandLine cmd;
	const char *args[] = {"progname", "--page=9 - 10", "-m first second"};
	char **argv = const_cast<char**>(args);
	cmd.parse(3, argv);

	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "9 - 10");
	EXPECT_TRUE(cmd.fontmapOpt.given());
	EXPECT_EQ(cmd.fontmapOpt.value(), "first second");
}


TEST(CommandLineTest, abbrev_long) {
	CommandLine cmd;
	const char *args[] = {"progname", "--pre=2", "--rot=-45.5", "--out=myfile.zyx", "--color"};
	char **argv = const_cast<char**>(args);
	cmd.parse(5, argv);

	EXPECT_TRUE(cmd.precisionOpt.given());
	EXPECT_EQ(cmd.precisionOpt.value(), 2);
	EXPECT_TRUE(cmd.rotateOpt.given());
	EXPECT_EQ(cmd.rotateOpt.value(), -45.5);
	EXPECT_TRUE(cmd.outputOpt.given());
	EXPECT_EQ(cmd.outputOpt.value(), "myfile.zyx");
	EXPECT_FALSE(cmd.bboxOpt.given());
	EXPECT_EQ(cmd.bboxOpt.value(), "min");
	EXPECT_EQ(cmd.filenames().size(), 0u);
	EXPECT_TRUE(cmd.colorOpt.given());
}


TEST(CommandLineTest, error) {
	CommandLine cmd;
	// error on wrong argument type
	const char *args1[] = {"progname", "--rotate=x"};
	char **argv = const_cast<char**>(args1);

	EXPECT_THROW(cmd.parse(2, argv), CL::CommandLineException);

	// error on missing arguments
	const char *args2[] = {"progname", "--page", "--zip"};
	argv = const_cast<char**>(args2);
	EXPECT_THROW(cmd.parse(3, argv), CL::CommandLineException);
	EXPECT_FALSE(cmd.zipOpt.given());
	EXPECT_EQ(cmd.zipOpt.value(), 9);
	EXPECT_FALSE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "1");
	EXPECT_FALSE(cmd.rotateOpt.given());
	EXPECT_EQ(cmd.filenames().size(), 0u);

	// error on ambiguous abbreviation of long option
	const char *args3[] = {"progname", "--no"};
	argv = const_cast<char**>(args3);
	EXPECT_THROW(cmd.parse(2, argv), CL::CommandLineException);

	// incorrect boolean value
	const char *args4[] = {"progname", "--trace-all=nope"};
	argv = const_cast<char**>(args4);
	EXPECT_THROW(cmd.parse(2, argv), CL::CommandLineException);
	EXPECT_FALSE(cmd.traceAllOpt.given());
}


TEST(CommandLineTest, file) {
	CommandLine cmd;
	const char *args1[] = {"progname", "--page=3", "-z5", "myfile1", "-l", "myfile2"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(6, argv);

	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "3");
	EXPECT_TRUE(cmd.zipOpt.given());
	EXPECT_EQ(cmd.zipOpt.value(), 5);
	EXPECT_TRUE(cmd.listSpecialsOpt.given());
	EXPECT_EQ(cmd.filenames().size(), 2u);
	EXPECT_EQ(cmd.filenames()[0], "myfile1");
	EXPECT_EQ(cmd.filenames()[1], "myfile2");
}


TEST(CommandLineTest, files_only) {
	CommandLine cmd;
	const char *args1[] = {"progname", "--page=3", "-z5", "--", "-l", "myfile"};
	char **argv = const_cast<char**>(args1);
	cmd.parse(6, argv);

	EXPECT_TRUE(cmd.pageOpt.given());
	EXPECT_EQ(cmd.pageOpt.value(), "3");
	EXPECT_TRUE(cmd.zipOpt.given());
	EXPECT_EQ(cmd.zipOpt.value(), 5);
	EXPECT_EQ(cmd.filenames().size(), 2u);
	EXPECT_EQ(cmd.filenames()[0], "-l");
	EXPECT_EQ(cmd.filenames()[1], "myfile");
}


TEST(CommandLineTest, help) {
	// only check whether help() succeeds
	CommandLine cmd;
	ostringstream oss;
	cmd.help(oss, 0);
	cmd.help(oss, 1);
	cmd.help(oss, 2);
}
