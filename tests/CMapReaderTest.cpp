/*************************************************************************
** CMapreaderTest.cpp                                                   **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "CMap.h"
#include "CMapReader.h"

using namespace std;

const char *cmap1 =
"/CIDInit /ProcSet findresource begin\n"
"12 dict begin\n"
"begincmap\n"
"/CIDSystemInfo 3 dict dup begin\n"
"  /Registry (Adobe) def\n"
"  /Ordering (Adobe_Japan1_UCS2) def\n"
"  /Supplement 6 def\n"
"end def\n"
"/CMapName /Test-Map def\n"
"/CMapVersion 8.002 def\n"
"/CMapType 2 def\n"
"/XUID [1 10 25335 1212] def\n"
"/WMode 0 def\n"
"1 begincodespacerange\n"
"  <0000> <FFFF>\n"
"endcodespacerange\n"
"4 beginbfchar\n"
"<0000> <fffd>\n"
"<003d> <00a5>\n"
"<005d> <00a6>\n"
"<005e> <007d>\n"
"endbfchar\n"
"2 beginbfchar\n"
"<005f> <0303>\n"
"<0060> <2019>\n"
"endbfchar\n"
"3 beginbfrange\n"
"<5830> <5831> <8c75>\n"
"<5833> <5834> <8c86>\n"
"<5837> <5838> <8c9b>\n"
"endbfrange\n"
"endcmap\n"
"CMapName currentdict /CMap defineresource pop\n";



TEST(CMapReader, read) {
	istringstream iss(cmap1);
	CMapReader reader;
	CMap *cmap = reader.read(iss, "Test-Map");
	ASSERT_TRUE(cmap != 0);
	ASSERT_EQ(cmap->getROString(), "Adobe-Adobe_Japan1_UCS2");
	ASSERT_FALSE(cmap->vertical());
	ASSERT_EQ(cmap->bfcode(0), 0xfffd);
	ASSERT_EQ(cmap->bfcode(0x3d), 0xa5);
	ASSERT_EQ(cmap->bfcode(0x5d), 0xa6);
	ASSERT_EQ(cmap->bfcode(0x5e), 0x7d);
	ASSERT_EQ(cmap->bfcode(0x5f), 0x303);
	ASSERT_EQ(cmap->bfcode(0x60), 0x2019);
	ASSERT_EQ(cmap->bfcode(0x5830), 0x8c75);
	ASSERT_EQ(cmap->bfcode(0x5831), 0x8c76);
	ASSERT_EQ(cmap->bfcode(0x5833), 0x8c86);
	ASSERT_EQ(cmap->bfcode(0x5838), 0x8c9c);
	ASSERT_EQ(cmap->bfcode(0x5839), 0);
}

