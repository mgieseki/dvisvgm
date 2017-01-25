/*************************************************************************
** FileFinderTest.cpp                                                   **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "FileFinder.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using std::ifstream;


class FileFinderTest : public ::testing::Test
{
	protected:
		void SetUp () override {
			FileFinder::init("FileFinderTest", "FileFinderTest", false);
			FileFinder::instance().addLookupDir(SRCDIR"/data");
		}
};


TEST_F(FileFinderTest, find_base_file) {
	const char *path = FileFinder::instance().lookup(SRCDIR"/FileFinderTest.cpp");
	EXPECT_TRUE(path);
	path = FileFinder::instance().lookup("Does-not-exist");
	EXPECT_FALSE(path);
	path = FileFinder::instance().lookup("frktest.dvi");
	EXPECT_TRUE(path);
	path = FileFinder::instance().lookup("cmr10.tfm");
	EXPECT_TRUE(path);
	ifstream ifs(path);
	EXPECT_TRUE(bool(ifs));
}


TEST_F(FileFinderTest, find_mapped_file) {
	// mapped base tfm file => should be resolved by kpathsea
	// circle10.tfm is usually mapped to lcircle.tfm
	if (const char *path = FileFinder::instance().lookup("circle10.tfm")) {
		EXPECT_TRUE(path);
		ifstream ifs(path);
		EXPECT_TRUE(bool(ifs));
	}

	// mapped lm font => should be resolved using dvisvgm's FontMap
	// cork-lmr10 is usually mapped to lmr10
	bool have_lmodern = FileFinder::instance().lookup("lmodern.sty");
	if (have_lmodern) {  // package lmodern installed?
		if (const char *path = FileFinder::instance().lookup("cork-lmr10.pfb")) {
			ifstream ifs(path);
			EXPECT_TRUE(bool(ifs));
		}
	}
}


TEST_F(FileFinderTest, mktexmf) {
	// ensure availability of ec font => call mktexmf if necessary
	if (const char *path = FileFinder::instance().lookup("ecrm2000.mf")) {
		ifstream ifs(path);
		EXPECT_TRUE(bool(ifs));
	}
}


TEST_F(FileFinderTest, find_unavailable_file) {
	const char *path = FileFinder::instance().lookup("not-available.xyz");
	EXPECT_FALSE(path);
}
