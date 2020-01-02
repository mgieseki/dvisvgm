/*************************************************************************
** GFGlyphTracerTest.cpp                                                **
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
#include "GFGlyphTracer.hpp"

#ifndef SRCDIR
#define SRCDIR "."
#endif

using namespace std;

class Callback : public GFGlyphTracer::Callback {
	public:
		void beginChar (uint8_t c) {_oss << "begin(" << int(c) << ")";}
		void endChar (uint8_t c)   {_oss << "end(" << int(c) << ")";}
		void emptyChar (uint8_t c) {_oss << "empty(" << int(c) << ")";}
		string getLog () const   {return _oss.str();}
		void reset ()            {_oss.str("");}

	private:
		ostringstream _oss;
};


// Returns an SVG path string with scaled values.
static string scaled_pathstr (const Glyph &glyph) {
	ostringstream oss;
	glyph.writeSVG(oss, false);
	istringstream iss(oss.str());
	ostringstream resultstream;
	XMLString::DECIMAL_PLACES = 1;
	while (iss.peek() > 0) {
		if (isalpha(iss.peek()) || isspace(iss.peek()))
			resultstream << char(iss.get());
		else {
			int num;
			iss >> num;
			resultstream << XMLString(double(num)/10000);
		}
	}
	return resultstream.str();
}


TEST(GFGlyphTracerTest, executeChar) {
	string gfname = string(SRCDIR)+"/data/cmr10.600gf";
	Callback callback;
	GFGlyphTracer tracer(gfname, 10000, &callback);
	Glyph glyph;
	tracer.setGlyph(glyph);
	EXPECT_FALSE(tracer.executeChar(128));
	EXPECT_EQ(callback.getLog(), "begin(128)empty(128)");

	callback.reset();
	EXPECT_TRUE(tracer.executeChar('g'));
	ostringstream oss;
	glyph.closeOpenSubPaths();
	EXPECT_EQ(scaled_pathstr(glyph),
		"M3.5 4.3C3.2 4.1 3.1 4.1 2.9 4.3C1.9 4.8 .6 4 .6 3C.6 2.8 .7 2.4 .8 2.3C.9 2.1 1 2 .9 1.7"
		"C.7 1.3 .7 .8 .9 .5C1 .3 1 .3 .6-.1C0-.7 .1-1.4 1.1-1.9C1.7-2.2 3.3-2.2 3.8-1.9"
		"C4.4-1.6 4.7-1.2 4.7-.8C4.7 .2 3.9 .7 2.4 .7C1.3 .7 1 .9 1.1 1.4C1.1 1.7 1.2 1.7 1.4 1.7"
		"C1.5 1.7 1.8 1.6 2 1.6C3.2 1.4 4.2 2.8 3.5 3.7C3.4 3.9 3.4 4 3.6 4.1C4 4.4 4.3 4.4 4.2 4.1"
		"C4.2 3.9 4.4 3.7 4.6 3.8C4.7 3.8 4.8 4 4.8 4.1C4.8 4.6 4.2 4.7 3.5 4.3Z"
		"M2.7 3.9C2.9 3.8 2.9 3.5 2.9 3C2.9 2.2 2.7 1.8 2.2 1.8C1.6 1.8 1.4 2.2 1.4 3C1.4 3.8 1.6 4.2 2.2 4.2C2.3 4.2 2.6 4.1 2.7 3.9Z"
		"M3.5-.1C4-.2 4.2-.7 4-1.1C3.6-2 1.7-2.2 1.1-1.4C.8-1 .8-.6 1.1-.2C1.3 .1 1.4 .1 2.2 .1C2.7 .1 3.3 0 3.5-.1Z");
	EXPECT_EQ(callback.getLog(), "begin(103)end(103)");

	tracer.reset(gfname, 1000);
	glyph.clear();
	callback.reset();
	EXPECT_TRUE(tracer.executeChar('I'));
	glyph.closeOpenSubPaths();
	oss.str("");
	EXPECT_EQ(scaled_pathstr(glyph),
		"M0 .7C0 .7 0 .6 .1 .6L.1 .6V.3V0L.1 0C0 0 0 0 0 0C0 0 .1 0 .2 0C.3 0 .3 0 .3 0"
		"C.3 0 .3 0 .3 0L.2 0V.3V.6L.3 .6C.3 .6 .3 .7 .3 .7C.3 .7 .3 .7 .2 .7C.1 .7 0 .7 0 .7Z");
	EXPECT_EQ(callback.getLog(), "begin(73)end(73)");
}


TEST(GFGlyphTracerTest, defaultCallback) {
	string gfname = string(SRCDIR)+"/data/cmr10.600gf";
	GFGlyphTracer::Callback callback;
	GFGlyphTracer tracer(gfname, 1000, &callback);
	Glyph glyph;
	tracer.setGlyph(glyph);
	tracer.closePath();
	EXPECT_FALSE(tracer.executeChar(128));
	EXPECT_TRUE(tracer.executeChar('g'));
}


TEST(GFGlyphTracerTest, fail) {
	GFGlyphTracer tracer;
	EXPECT_FALSE(tracer.executeChar('M'));

	Glyph glyph;
	tracer.setGlyph(glyph);
	EXPECT_THROW(tracer.executeChar('M'), GFException);

	string gfname = string(SRCDIR)+"/data/cmr10.600gf";
	tracer.reset(gfname, 1000);
	EXPECT_TRUE(tracer.executeChar('M'));
}
