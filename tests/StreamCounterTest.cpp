/*************************************************************************
** StreamCounterTest.cpp                                                **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <sstream>
#include "StreamCounter.h"


TEST(StreamCounterTest, count) {
	std::ostringstream ss;
	StreamCounter<char> sc(ss);
	EXPECT_EQ(sc.count(), unsigned(0));

	ss << "0123456789";
	EXPECT_EQ(sc.count(), unsigned(10));
	
	ss << "0123456789";
	EXPECT_EQ(sc.count(), unsigned(20));

	sc.reset();
	EXPECT_EQ(sc.count(), unsigned(0));
}
