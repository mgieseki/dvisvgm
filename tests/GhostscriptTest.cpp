/*************************************************************************
** GhostscriptTest.cpp                                                  **
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
#include "Ghostscript.hpp"


TEST(GhostscriptTest, available1) {
	const char *args[] = {"test", "-q", "-dNODISPLAY"};
	Ghostscript gs(3, args);
	ASSERT_TRUE(gs.available());
}


TEST(GhostscriptTest, available2) {
	Ghostscript gs;
	ASSERT_TRUE(gs.available());
}


TEST(GhostscriptTest, revision) {
	Ghostscript gs;
	ASSERT_GT(gs.revision(), 0);
	ASSERT_FALSE(gs.revisionstr().empty());
}


TEST(Ghostscript, error_name) {
	Ghostscript gs;
	ASSERT_EQ(gs.error_name(0), (const char*)0);
	ASSERT_STREQ(gs.error_name(-1), "unknownerror");
	ASSERT_STREQ(gs.error_name(1), "unknownerror");
	ASSERT_STREQ(gs.error_name(2), "dictfull");
}
