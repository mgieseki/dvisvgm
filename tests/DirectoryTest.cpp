/*************************************************************************
** DirectoryTest.cpp                                                    **
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
#include <iostream>
#include <set>
#include <unistd.h>
#include "Directory.h"
#include "FileSystem.h"
#include "macros.h"

#ifndef SRCDIR
#define SRCDIR "."
#endif


using namespace std;

TEST(DirectoryTest, dirs) {
	const char *dirs_to_find[] = {"doc", "src", 0};
	set<string> found_dirs;
	Directory dir("..");
	while (const char *dname = dir.read('d'))
		found_dirs.insert(dname);
	for (const char **p=dirs_to_find; *p; ++p) {
		EXPECT_TRUE(found_dirs.find(*p) != found_dirs.end());
	}
}


TEST(DirectoryTest, file) {
	const char *files_to_find[] = {
		"Bitmap.cpp", "BoundingBox.cpp", "Calculator.cpp",
		"Directory.cpp", "DVIReader.cpp", "DVIToSVG.cpp", "DVIToSVGActions.cpp",
		"FileSystem.cpp", "Font.cpp", "FontEngine.cpp", "FontManager.cpp",
		"FontMap.cpp", "GFReader.cpp", "GFTracer.cpp", "FileFinder.cpp", "Message.cpp",
		"MetafontWrapper.cpp", "PageSize.cpp", "StreamReader.cpp",
		"TFM.cpp", "VFReader.cpp", "XMLDocument.cpp", "XMLNode.cpp", "XMLString.cpp",
		0
	};
	set<string> found_files;
	Directory dir(string(SRCDIR)+"/../src");
	while (const char *fname = dir.read('f'))
		found_files.insert(fname);
	for (const char **p=files_to_find; *p; ++p) {
		EXPECT_TRUE(found_files.find(*p) != found_files.end());
	}
	EXPECT_TRUE(found_files.find("tests") == found_files.end());
}
