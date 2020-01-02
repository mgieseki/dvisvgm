/*************************************************************************
** SpecialManagerTest.cpp                                               **
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
#include <sstream>
#include "SpecialManager.hpp"
#include "BgColorSpecialHandler.hpp"
#include "ColorSpecialHandler.hpp"
#include "DvisvgmSpecialHandler.hpp"
#include "EmSpecialHandler.hpp"
#include "HtmlSpecialHandler.hpp"
#include "NoPsSpecialHandler.hpp"
#include "PapersizeSpecialHandler.hpp"
#include "PdfSpecialHandler.hpp"
#include "TpicSpecialHandler.hpp"
#include "utility.hpp"

using namespace std;

class SpecialManagerTest : public ::testing::Test {
	public:
		SpecialManagerTest () {
			handlers.emplace_back(util::make_unique<BgColorSpecialHandler>());
			handlers.emplace_back(util::make_unique<ColorSpecialHandler>());
			handlers.emplace_back(util::make_unique<DvisvgmSpecialHandler>());
			handlers.emplace_back(util::make_unique<EmSpecialHandler>());
			handlers.emplace_back(util::make_unique<HtmlSpecialHandler>());
			handlers.emplace_back(util::make_unique<PapersizeSpecialHandler>());
			handlers.emplace_back(util::make_unique<PdfSpecialHandler>());
			handlers.emplace_back(util::make_unique<TpicSpecialHandler>());
			handlers.emplace_back(util::make_unique<NoPsSpecialHandler>());
		}

		vector<unique_ptr<SpecialHandler>> handlers;
};


TEST_F(SpecialManagerTest, info1) {
	SpecialManager &sm = SpecialManager::instance();
	sm.unregisterHandlers();
	sm.registerHandlers(handlers, "");
	ostringstream oss;
	sm.writeHandlerInfo(oss);
	string expected =
		"bgcolor    background color special\n"
		"color      complete support of color specials\n"
		"dvisvgm    special set for embedding raw SVG snippets\n"
		"em         line drawing statements of the emTeX special set\n"
		"html       hyperref specials\n"
		"papersize  special to set the page size\n"
		"pdf        PDF hyperlink, font map, and pagesize specials\n"
		"tpic       TPIC specials\n";
	EXPECT_EQ(oss.str(), expected);
}


TEST_F(SpecialManagerTest, info2) {
	SpecialManager &sm = SpecialManager::instance();
	sm.unregisterHandlers();
	sm.registerHandlers(handlers, "color,em");
	ostringstream oss;
	sm.writeHandlerInfo(oss);
	string expected =
		"bgcolor    background color special\n"
		"dvisvgm    special set for embedding raw SVG snippets\n"
		"html       hyperref specials\n"
		"papersize  special to set the page size\n"
		"pdf        PDF hyperlink, font map, and pagesize specials\n"
		"tpic       TPIC specials\n";
	EXPECT_EQ(oss.str(), expected);
}
