/*************************************************************************
** MessageExceptionTest.cpp                                             **
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
#include "MessageException.hpp"

using namespace std;

static void throw_exception (const string &msg) {
	throw MessageException(msg);
}


TEST(MessageExceptionTest, catch_direct) {
	bool caught=false;
	try {
		throw_exception("test message");
		FAIL();
	}
	catch (MessageException &e) {
		ASSERT_STREQ(e.what(), "test message");
		caught = true;
	}
	catch (...) {
		FAIL();
	}
	ASSERT_TRUE(caught);
}


TEST(MessageExceptionTest, catch_indirect1) {
	bool caught=false;
	try {
		throw_exception("test message");
		FAIL();
	}
	catch (exception &e) {
		ASSERT_STREQ(e.what(), "test message");
		caught = true;
	}
	catch (...) {
		FAIL();
	}
	ASSERT_TRUE(caught);
}


TEST(MessageExceptionTest, catch_indirect2) {
	bool caught=false;
	try {
		throw_exception("test message");
		FAIL();
	}
	catch (...) {
		caught = true;
	}
	ASSERT_TRUE(caught);
}

