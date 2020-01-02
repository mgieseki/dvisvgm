/*************************************************************************
** FontCacheTest.cpp                                                    **
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
#include <sstream>
#include "FileSystem.hpp"
#include "FontCache.hpp"

#ifndef BUILDDIR
#define BUILDDIR "."
#endif

using namespace std;

class LocalCache {
	public:
		LocalCache (const string &cachedir)
			: _cachedir(cachedir),
			  _created(!FileSystem::exists(cachedir) && FileSystem::mkdir(cachedir))
		{
		}

		~LocalCache () {
			if (_created)
				FileSystem::rmdir(_cachedir);
		}

		string cachedir () const {return _cachedir;}

	private:
		string _cachedir;
		bool _created;
};


static LocalCache localCache(BUILDDIR"/data");


class FontCacheTest : public testing::Test {
	protected:
		FontCacheTest () : testing::Test(), cachedir(localCache.cachedir()) {
			glyph1.moveto(0, 0);
			glyph1.lineto(10, 0);
			glyph1.lineto(10, 10);
			glyph1.lineto(0, 10);
			glyph1.closepath();

			glyph2.moveto(0, 0);
			glyph2.cubicto(10, 10, 20, 0, 50, 50);
			glyph2.lineto(30, 20);
			glyph2.quadto(20, 40, 20, 20);
			glyph2.closepath();
		}

		~FontCacheTest () override {
			FileSystem::remove(cachedir+"/testfont.fgd");
		}

		Glyph glyph1, glyph2;
		FontCache cache;
		string cachedir;
};


static string toSVG (const Glyph &glyph) {
	ostringstream oss;
	glyph.writeSVG(oss, false);
	return oss.str();
}


TEST_F(FontCacheTest, glyph) {
	EXPECT_EQ(toSVG(glyph1), "M0 0H10V10H0Z");
	EXPECT_EQ(toSVG(glyph2), "M0 0C10 10 20 0 50 50L30 20Q20 40 20 20Z");
	cache.setGlyph(1, glyph1);
	cache.setGlyph(10, glyph2);
	ASSERT_NE(cache.getGlyph(1), nullptr);
	ASSERT_EQ(cache.getGlyph(2), nullptr);
	ASSERT_NE(cache.getGlyph(10), nullptr);
	ASSERT_EQ(*cache.getGlyph(1), glyph1);
	ASSERT_EQ(*cache.getGlyph(10), glyph2);
}


TEST_F(FontCacheTest, write1) {
	cache.setGlyph(1, glyph1);
	ASSERT_TRUE(cache.fontname().empty());
	ASSERT_FALSE(cache.write(cachedir));
}


TEST_F(FontCacheTest, write2) {
	cache.setGlyph(1, glyph1);
	ASSERT_TRUE(FileSystem::exists(cachedir));
	ASSERT_TRUE(cache.write("testfont", cachedir));
	cache.setGlyph(10, glyph2);
	EXPECT_TRUE(cache.write("testfont", cachedir));
	EXPECT_TRUE(cache.fontname().empty());
}


TEST_F(FontCacheTest, read) {
	cache.setGlyph(1, glyph1);
	cache.setGlyph(10, glyph2);
	ASSERT_TRUE(cache.write("testfont", cachedir));
	// clear cache object
	cache.clear();
	EXPECT_EQ(cache.getGlyph(1), nullptr);
	EXPECT_EQ(cache.getGlyph(2), nullptr);
	EXPECT_EQ(cache.getGlyph(10), nullptr);
	// read glyph data from cache file
	ASSERT_TRUE(cache.read("testfont", cachedir));
	EXPECT_EQ(cache.fontname(), "testfont");
	ASSERT_NE(cache.getGlyph(1), nullptr);
	EXPECT_EQ(cache.getGlyph(2), nullptr);
	ASSERT_NE(cache.getGlyph(10), nullptr);
	EXPECT_EQ(*cache.getGlyph(1), glyph1);
	EXPECT_EQ(*cache.getGlyph(10), glyph2);
}


TEST_F(FontCacheTest, fontinfo1) {
	ostringstream oss;
	cache.clear();
	FileSystem::remove(cachedir+"/testfont.fgd");
	cache.fontinfo(cachedir, oss);
	ASSERT_EQ(oss.str(), "cache is empty\n");

	// check removal of invalid cache files
	ofstream cachefile(cachedir+"/invalid.fgd");
	cachefile << "invalid cache file";
	cachefile.close();
	ASSERT_TRUE(FileSystem::exists(cachedir+"/invalid.fgd"));
	oss.str("");
	cache.fontinfo(cachedir, oss, true);
	ASSERT_EQ(oss.str(),
		"cache is empty\n"
		"invalid cache file invalid.fgd removed\n"
	);
	ASSERT_FALSE(FileSystem::exists(cachedir+"/invalid.fgd"));
}


TEST_F(FontCacheTest, fontinfo2) {
	cache.setGlyph(1, glyph1);
	cache.setGlyph(10, glyph2);
	ASSERT_TRUE(cache.write("testfont", cachedir));

	ostringstream oss;
	cache.fontinfo(cachedir, oss);
	ASSERT_EQ(oss.str(),
		"cache format version 5\n"
		"testfont      2 glyphs        10 cmds          58 bytes  hash:3cb32ab6\n"
	);
}
