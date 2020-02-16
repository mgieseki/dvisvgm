/*************************************************************************
** FilePathTest.cpp                                                     **
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
#include <string>
#include "FilePath.hpp"
#include "FileSystem.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


TEST(FilePathTest, empty) {
	FilePath path;
	ASSERT_TRUE(path.empty());
	path.set("/a/b/c/d", false, "/");
	ASSERT_FALSE(path.empty());
	ASSERT_EQ(path.absolute(), "/a/b/c/d");
}


TEST(FilePathTest, dir1) {
	FilePath fp("a/b/c/d", false, "/");
	ASSERT_EQ(fp.absolute(), "/a/b/c/d");
	ASSERT_EQ(fp.relative("/"), "a/b/c/d");
	ASSERT_EQ(fp.relative("/a/b"), "c/d");
	ASSERT_EQ(fp.relative("/a/b/c"), "d");
	ASSERT_EQ(fp.relative("/a/b/c/d"), ".");
	ASSERT_EQ(fp.relative("/a/b/x"), "../c/d");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d");
}


TEST(FilePathTest, dir2) {
	FilePath fp("a/b/c/d", false, "/x/y");
	ASSERT_EQ(fp.absolute(), "/x/y/a/b/c/d");
	ASSERT_EQ(fp.relative("/"), "x/y/a/b/c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b"), "c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b/c"), "d");
	ASSERT_EQ(fp.relative("/x/y/a/b/c/d"), ".");
	ASSERT_EQ(fp.relative("/x/y/a/b/x"), "../c/d");
	ASSERT_EQ(fp.relative("/x/y/a/b/x/y"), "../../c/d");
}


TEST(FilePathTest, file1) {
	FilePath fp("a/b/c/d/f.ext", true, "/");
	ASSERT_EQ(fp.absolute(), "/a/b/c/d/f.ext");
	ASSERT_EQ(fp.relative("/"), "a/b/c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b"), "c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/c"), "d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/c/d"), "f.ext");
	ASSERT_EQ(fp.relative("/a/b/x"), "../c/d/f.ext");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d/f.ext");
	ASSERT_EQ(fp.basename(), "f");
	ASSERT_EQ(fp.suffix(), "ext");
	fp.suffix("new");
	ASSERT_EQ(fp.suffix(), "new");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d/f.new");
	fp.suffix("");
	ASSERT_EQ(fp.suffix(), "");
	ASSERT_EQ(fp.relative("/a/b/x/y"), "../../c/d/f");
}


TEST(FilePathTest, file2) {
	FilePath fp("/f.ext", true, "/");
	ASSERT_EQ(fp.absolute(), "/f.ext");
	ASSERT_EQ(fp.relative("/a/b"), "../../f.ext");
}


TEST(FilePathTest, autodetect) {
	FileSystem::chdir(SRCDIR);
	FilePath fp1("FilePathTest.cpp");
	ASSERT_TRUE(fp1.isFile());
	ASSERT_FALSE(fp1.empty());
	string cwd = FileSystem::getcwd();
	ASSERT_EQ(fp1.absolute(), cwd + "/FilePathTest.cpp") << "fp1=" << fp1.absolute();

	FilePath fp2("");
	ASSERT_FALSE(fp2.isFile());
	ASSERT_FALSE(fp2.empty());
	ASSERT_EQ(fp2.absolute(), FileSystem::getcwd());
}
