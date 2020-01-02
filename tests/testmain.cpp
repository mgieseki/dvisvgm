/*************************************************************************
** testmain.cpp                                                         **
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

#include <string>
#include <gtest/gtest.h>
#include "FileFinder.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

std::string TEST_ARGV0;

struct TestEnvironment : public testing::Environment {
	void SetUp () override {
		FileFinder::init(TEST_ARGV0, "dvisvgm-test", false);
		FileFinder::instance().addLookupDir(SRCDIR);
		FileFinder::instance().addLookupDir(SRCDIR"/data");
	}
};

GTEST_API_ int main (int argc, char **argv) {
	TEST_ARGV0 = argv[0];
	testing::InitGoogleTest(&argc, argv);
	testing::AddGlobalTestEnvironment(new TestEnvironment);
	return RUN_ALL_TESTS();
}
