/*************************************************************************
** FileSystemTest.cpp                                                   **
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

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;


TEST(FileSystemTest, exists) {
	EXPECT_TRUE(FileSystem::exists(SRCDIR"/FileSystemTest.cpp"));
	EXPECT_FALSE(FileSystem::exists(SRCDIR"/FileSystemTest.cxx"));
}


TEST(FileSystemTest, filetype) {
	EXPECT_TRUE(FileSystem::isFile(SRCDIR"/FileSystemTest.cpp"));
	EXPECT_FALSE(FileSystem::isDirectory(SRCDIR"/FileSystemTest.cpp"));
	EXPECT_TRUE(FileSystem::isDirectory("../tests"));
	EXPECT_FALSE(FileSystem::isFile("../tests"));
}


TEST(FileSystemTest, remove) {
	const char *tmpfile = "out.tmp";
	ofstream ofs(tmpfile);
	ofs << "FileSystemTest::remove\n";
	ofs.close();
	EXPECT_TRUE(FileSystem::exists(tmpfile));
	FileSystem::remove(tmpfile);
	EXPECT_FALSE(FileSystem::exists(tmpfile));
}


TEST(FileSystemTest, rename) {
	const char *tmpfile1 = "out.tmp";
	const char *tmpfile2 = "out-new.tmp";
	ofstream ofs(tmpfile1);
	ofs << "FileSystemTest::remove\n";
	ofs.close();
	EXPECT_TRUE(FileSystem::exists(tmpfile1));
	FileSystem::rename(tmpfile1, tmpfile2);
#ifndef __WIN32__
	EXPECT_FALSE(FileSystem::exists(tmpfile1));
#endif
	EXPECT_TRUE(FileSystem::exists(tmpfile2));
	FileSystem::remove(tmpfile2);
	EXPECT_FALSE(FileSystem::exists(tmpfile2));
}


TEST(FileSystemTest, filesize) {
	const char *tmpfile = "out.tmp";
	ofstream ofs(tmpfile, ios::binary);
	for (int i=0; i < 123; ++i)
		ofs.put(i);
	ofs.close();
	EXPECT_EQ(FileSystem::filesize(tmpfile), 123u);
	FileSystem::remove(tmpfile);
}


TEST(FileSystemTest, copy) {
	const char *tmpfile1 = "out.tmp";
	const char *tmpfile2 = "out-new.tmp";
	ofstream ofs(tmpfile1);
	ofs << "FileSystemTest::copy\n";
	ofs.close();
	FileSystem::copy(tmpfile1, tmpfile2);
	ifstream ifs1(tmpfile1);
	ifstream ifs2(tmpfile2);
	EXPECT_TRUE(ifs1 && ifs2);
	while (!ifs1.eof()) {
		EXPECT_EQ(ifs1.get(), ifs2.get());
	}
	EXPECT_TRUE(ifs2.eof());
	ifs1.close();
	ifs2.close();
	FileSystem::remove(tmpfile1);
	FileSystem::remove(tmpfile2);
}


TEST(FileSystemTest, move) {
	const char *tmpfile1 = "out.tmp";
	const char *tmpfile2 = "out-new.tmp";
	ofstream ofs(tmpfile1);
	const char *str = "FileSystemTest::copy\n";
	ofs << str;
	ofs.close();
	FileSystem::copy(tmpfile1, tmpfile2, true);
	ifstream ifs1(tmpfile1);
	ifstream ifs2(tmpfile2);
	EXPECT_TRUE(!ifs1 && ifs2);
	while (*str) {
		EXPECT_EQ(*str++, ifs2.get());
	}
	ifs2.get();
	EXPECT_TRUE(ifs2.eof());
	ifs2.close();
	FileSystem::remove(tmpfile2);
}
