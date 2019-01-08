/*************************************************************************
** SVGOutputTest.cpp                                                    **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2019 Martin Gieseking <martin.gieseking@uos.de>   **
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
	EXPECT_EQ(out.filename(1, 1), "SVGOutputTest.svg");
	EXPECT_EQ(out.filename(5, 9), "SVGOutputTest-5.svg");
	EXPECT_EQ(out.filename(5, 10), "SVGOutputTest-05.svg");
	EXPECT_EQ(out.filename(5, 256), "SVGOutputTest-005.svg");
}


TEST_F(SVGOutputTest, widthSpecifier) {
	{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--005.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--054.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--543.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p--%P");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--005--9.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--054--65.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--543--654.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3p--%3P");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--005--009.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--054--065.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--543--654.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%5f--%3p--%3P");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--005--009.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--054--065.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--543--654.svg");
	}
}


TEST_F(SVGOutputTest, expressions) {
	{
		SVGOutput out("SVGOutputTest.cpp", "no-macro");
		EXPECT_EQ(out.filename(5, 9), "no-macro.svg");
		EXPECT_EQ(out.filename(54, 65), "no-macro.svg");
		EXPECT_EQ(out.filename(543, 654), "no-macro.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%(p-1)");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--4.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--53.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--542.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3(p-1)");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--004.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--053.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--542.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f--%3(P+2p)");
		EXPECT_EQ(out.filename(5, 9), "SVGOutputTest--019.svg");
		EXPECT_EQ(out.filename(54, 65), "SVGOutputTest--173.svg");
		EXPECT_EQ(out.filename(543, 654), "SVGOutputTest--1740.svg");
	}
}


TEST_F(SVGOutputTest, hashes) {
	SVGOutput::HashTriple hashes("dvihash", "opthash", "cmbhash");
	{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x");
		EXPECT_EQ(out.filename(1, 10), "SVGOutputTest--x.svg");
		EXPECT_EQ(out.filename(1, 10, hashes), "SVGOutputTest-dvihash-x.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x-%hc%ho");
		EXPECT_EQ(out.filename(1, 10), "SVGOutputTest--x-.svg");
		EXPECT_EQ(out.filename(1, 10, hashes), "SVGOutputTest-dvihash-x-cmbhashopthash.svg");
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd%p%ho");
		EXPECT_EQ(out.filename(1, 10), "SVGOutputTest-01.svg");
		EXPECT_EQ(out.filename(1, 10, hashes), "SVGOutputTest-dvihash01opthash.svg");
	}
}


TEST_F(SVGOutputTest, hashes_fail) {
	SVGOutput::HashTriple hashes("dvihash", "opthash", "cmbhash");
	{
		SVGOutput out("SVGOutputTest.cpp", "%f-%h-x");
		EXPECT_THROW(out.filename(1, 10), MessageException);
		EXPECT_THROW(out.filename(1, 10, hashes), MessageException);
	}{
		SVGOutput out("SVGOutputTest.cpp", "%f-%hd-x-%ha%ho");
		EXPECT_THROW(out.filename(1, 10), MessageException);
		EXPECT_THROW(out.filename(1, 10, hashes), MessageException);
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
	EXPECT_EQ(out.filename(5, 9), "SVGOutputTest-5.svg");
}


TEST_F(SVGOutputTest, error) {
	SVGOutput out("SVGOutputTest.cpp", "%(p/0)");
	EXPECT_THROW(out.filename(5, 9), MessageException);
}
