/***********************************************************************
** DirectoryTest.h                                                    **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/

#include <cxxtest/TestSuite.h>
#include <set>
#include <unistd.h>
#include "Directory.h"
#include "FileSystem.h"
#include "macros.h"

using std::set;
using std::string;

class DirectoryTest : public CxxTest::TestSuite
{
	public:
		void test_dirs () {
			const char *dirs_to_find[] = {"doc", "rpm", "src", 0};
			set<string> found_dirs;
			Directory dir("..");
			while (const char *dname = dir.read('d'))
				found_dirs.insert(dname);
			for (const char **p=dirs_to_find; *p; ++p) {
				TS_ASSERT(found_dirs.find(*p) != found_dirs.end());
			}
		}


		void test_file () {
			// Skip this test if test-all is executed outside the original src directory.
			// This is necessary to make "make distcheck" happy.
			string cwd = FileSystem::getcwd();
			unsigned pos = cwd.find("dvisvgm/src");
			if (pos != cwd.length()-11) {
				TS_WARN("DirectoryTest::test_file skipped");
				return;
			}

			const char *files_to_find[] = {
				"Bitmap.cpp", "BoundingBox.cpp", "Calculator.cpp", "CharmapTranslator.cpp", 
				"Directory.cpp", "DVIReader.cpp", "DVIToSVG.cpp", "DVIToSVGActions.cpp", 
				"FileSystem.cpp", "Font.cpp", "FontEngine.cpp", "FontGlyph.cpp", "FontManager.cpp", 
				"FontMap.cpp", "GFReader.cpp", "GFTracer.cpp", "FileFinder.cpp", "Message.cpp", 
				"MetafontWrapper.cpp", "PageSize.cpp", "StreamReader.cpp", "SVGFontEmitter.cpp", 
				"SVGFontTraceEmitter.cpp", "TFM.cpp", "TransformationMatrix.cpp", "VFReader.cpp", 
				"XMLDocument.cpp", "XMLNode.cpp", "XMLString.cpp",
				0
			};
			set<string> found_files;
			Directory dir(".");
			while (const char *fname = dir.read('f'))
				found_files.insert(fname);
			for (const char **p=files_to_find; *p; ++p) {
				TS_ASSERT(found_files.find(*p) != found_files.end());
			}
			TS_ASSERT(found_files.find("tests") == found_files.end());
		}
};
