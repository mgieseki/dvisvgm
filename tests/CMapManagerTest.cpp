/*************************************************************************
** CMapManagerTest.cpp                                                  **
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
#include "CMap.hpp"
#include "CMapManager.hpp"
#include "Message.hpp"


TEST(CMapManagerTest, lookup_buildin) {
	CMapManager &manager = CMapManager::instance();
	CMap *cmap=0;
	ASSERT_NE((cmap = manager.lookup("Identity-H")), nullptr);
	EXPECT_STREQ(cmap->name(), "Identity-H");
	EXPECT_EQ(cmap->getROString(), "Adobe-Identity");
	EXPECT_TRUE(cmap->mapsToCID());

	ASSERT_NE((cmap = manager.lookup("Identity-V")), nullptr);
	EXPECT_STREQ(cmap->name(), "Identity-V");
	EXPECT_EQ(cmap->getROString(), "Adobe-Identity");
	EXPECT_TRUE(cmap->mapsToCID());

	ASSERT_NE((cmap = manager.lookup("unicode")), nullptr);
	EXPECT_STREQ(cmap->name(), "unicode");
	EXPECT_EQ(cmap->getROString(), "");
	EXPECT_FALSE(cmap->mapsToCID());
	EXPECT_EQ(cmap->path(), nullptr);
}


TEST(CMapManagerTest, lookup_file) {
	CMapManager &manager = CMapManager::instance();
	CMap *cmap = manager.lookup("ot1.cmap");
	ASSERT_NE(cmap, nullptr);
	EXPECT_STREQ(cmap->name(), "ot1.cmap");
	EXPECT_EQ(cmap->getROString(), "");
	EXPECT_FALSE(cmap->mapsToCID());
	EXPECT_EQ(cmap->bfcode(0x00), 0x0393u);
	EXPECT_EQ(cmap->bfcode(0x01), 0x0394u);
	EXPECT_EQ(cmap->bfcode(0x02), 0x0398u);
	EXPECT_EQ(cmap->bfcode(0x7c), 0x2014u);
	EXPECT_EQ(cmap->bfcode(0x7d), 0x02ddu);
	EXPECT_EQ(cmap->bfcode(0x7f), 0x00a8u);
	EXPECT_EQ(cmap->bfcode(0x80), 0u);
}


TEST(CMapManagerTest, lookup_fail) {
	Message::LEVEL = 0;  // avoid warning messages
	CMapManager &manager = CMapManager::instance();
	EXPECT_EQ(manager.lookup("does-not-exists"), nullptr);
}
