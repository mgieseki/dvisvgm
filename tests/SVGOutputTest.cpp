/*************************************************************************
** SVGOutputTest.cpp                                                    **
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
#include <fstream>
#include "FileSystem.hpp"
#include "MessageException.hpp"
#include "SVGOutput.hpp"
#include "ZLibOutputStream.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;

struct SVGOutputTest : public ::testing::Test {
	void SetUp () override {FileSystem::chdir(SRCDIR);}
};


TEST_F(SVGOutputTest, defaults) {
	SVGOutput out("SVGOutputTest.cpp", "");
	EXPECT_EQ(out.filepath(1, 1).relative(), "SVGOutputTest.svg");
	EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest-5.svg");
	EXPECT_EQ(out.filepath(5, 10).relative(), "SVGOutputTest-05.svg");
	EXPECT_EQ(out.filepath(5, 256).relative(), "SVGOutputTest-005.svg");
}


TEST_F(SVGOutputTest, widthSpecifier) {
	{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--005.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--054.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--543.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p--%P");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--005--9.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--054--65.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--543--654.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p--%3P");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--005--009.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--054--065.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--543--654.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%5f--%3p--%3P");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--005--009.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--054--065.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--543--654.svg");
	}
}


TEST_F(SVGOutputTest, expressions) {
	{
		SVGOutput out("SVGOutputTest.cpp", "no-macro");
		EXPECT_EQ(out.filepath(5, 9).relative(), "no-macro.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "no-macro.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "no-macro.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%(p-1)");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--4.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--53.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--542.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3(p-1)");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--004.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--053.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--542.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3(P+2p)");
		EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest--019.svg");
		EXPECT_EQ(out.filepath(54, 65).relative(), "SVGOutputTest--173.svg");
		EXPECT_EQ(out.filepath(543, 654).relative(), "SVGOutputTest--1740.svg");
	}
}


TEST_F(SVGOutputTest, hashes) {
	SVGOutput::HashTriple hashes("dvihash", "opthash", "cmbhash");
	{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x");
		EXPECT_EQ(out.filepath(1, 10).relative(), "SVGOutputTest--x.svg");
		EXPECT_EQ(out.filepath(1, 10, hashes).relative(), "SVGOutputTest-dvihash-x.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x-%hc%ho");
		EXPECT_EQ(out.filepath(1, 10).relative(), "SVGOutputTest--x-.svg");
		EXPECT_EQ(out.filepath(1, 10, hashes).relative(), "SVGOutputTest-dvihash-x-cmbhashopthash.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd%p%ho");
		EXPECT_EQ(out.filepath(1, 10).relative(), "SVGOutputTest-01.svg");
		EXPECT_EQ(out.filepath(1, 10, hashes).relative(), "SVGOutputTest-dvihash01opthash.svg");
	}
}


TEST_F(SVGOutputTest, hashes_fail) {
	SVGOutput::HashTriple hashes("dvihash", "opthash", "cmbhash");
	{
		SVGOutput out("SVGOutputTest.cpp", "%f-%h-x");
		EXPECT_THROW(out.filepath(1, 10).relative(), MessageException);
		EXPECT_THROW(out.filepath(1, 10, hashes).relative(), MessageException);
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x-%ha%ho");
		EXPECT_THROW(out.filepath(1, 10).relative(), MessageException);
		EXPECT_THROW(out.filepath(1, 10, hashes).relative(), MessageException);
	}
}


TEST_F(SVGOutputTest, getPageStream) {
	{
		SVGOutput out("", "");
		ostream &os = out.getPageStream(1, 10);
		EXPECT_EQ(&os, &cout);
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%p");
		ostream *os1 = &out.getPageStream(1, 10);
		EXPECT_TRUE(dynamic_cast<ofstream*>(os1));
		ostream *os2 = &out.getPageStream(1, 10);
		EXPECT_EQ(os1, os2);
	}
	FileSystem::remove("SVGOutputTest-01.svg");
	{
		SVGOutput out("SVGOutputTest.cpp", "%f-%p", 9);
		ostream *os = &out.getPageStream(1, 10);
		EXPECT_TRUE(dynamic_cast<ZLibOutputStream*>(os));
	}
	FileSystem::remove("SVGOutputTest-01.svgz");
}


TEST_F(SVGOutputTest, ignore) {
	SVGOutput out("SVGOutputTest.cpp", "%x %y");
	EXPECT_EQ(out.filepath(5, 9).relative(), "SVGOutputTest-5.svg");
}


TEST_F(SVGOutputTest, error) {
	SVGOutput out("SVGOutputTest.cpp", "%(p/0)");
	EXPECT_THROW(out.filepath(5, 9).relative(), MessageException);
}
