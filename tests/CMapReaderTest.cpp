/*************************************************************************
** CMapReaderTest.cpp                                                   **
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
#include <memory>
#include "CMap.hpp"
#include "CMapReader.hpp"

using namespace std;


class CMapReaderTest : public ::testing::Test
{
	protected:
		CMapReaderTest () {
			istringstream iss(cmapsrc);
			CMapReader reader;
			cmap = reader.read(iss, "Test-Map");
		}

		static const char *cmapsrc;
		unique_ptr<CMap> cmap;
};


const char *CMapReaderTest::cmapsrc =
"% This is a comment line\n"
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
"3 begincidchar\n"
"<1000> 50\n"
"<1005> 60\n"
"<1008> 70\n"
"endcidchar\n"
"2 begincidrange\n"
"<1234> <1240> 100\n"
"<1300> <1302> 200\n"
"endcidrange\n"
"endcmap\n"
"CMapName currentdict /CMap defineresource pop\n";


TEST_F(CMapReaderTest, rostring) {
	ASSERT_NE(cmap, nullptr);
	ASSERT_EQ(cmap->getROString(), "Adobe-Adobe_Japan1_UCS2");
	ASSERT_FALSE(cmap->vertical());
}


TEST_F(CMapReaderTest, bfcode) {
	ASSERT_NE(cmap, nullptr);
	ASSERT_EQ(cmap->getROString(), "Adobe-Adobe_Japan1_UCS2");
	ASSERT_FALSE(cmap->vertical());
	ASSERT_EQ(cmap->bfcode(0), 0xfffdu);
	ASSERT_EQ(cmap->bfcode(0x3d), 0xa5u);
	ASSERT_EQ(cmap->bfcode(0x5d), 0xa6u);
	ASSERT_EQ(cmap->bfcode(0x5e), 0x7du);
	ASSERT_EQ(cmap->bfcode(0x5f), 0x303u);
	ASSERT_EQ(cmap->bfcode(0x60), 0x2019u);
	ASSERT_EQ(cmap->bfcode(0x5830), 0x8c75u);
	ASSERT_EQ(cmap->bfcode(0x5831), 0x8c76u);
	ASSERT_EQ(cmap->bfcode(0x5833), 0x8c86u);
	ASSERT_EQ(cmap->bfcode(0x5838), 0x8c9cu);
	ASSERT_EQ(cmap->bfcode(0x5839), 0u);
}

TEST_F(CMapReaderTest, cid) {
	const SegmentedCMap *seg_cmap = dynamic_cast<const SegmentedCMap*>(cmap.get());
	ASSERT_NE(seg_cmap, nullptr);
	ASSERT_EQ(seg_cmap->numBFRanges(), 9u);
	ASSERT_EQ(seg_cmap->numCIDRanges(), 5u);
	ASSERT_EQ(seg_cmap->cid(0x1000), 50u);
	ASSERT_EQ(seg_cmap->cid(0x1005), 60u);
	ASSERT_EQ(seg_cmap->cid(0x1008), 70u);

	ASSERT_EQ(seg_cmap->cid(0x1233), 0u);
	ASSERT_EQ(seg_cmap->cid(0x1234), 100u);
	ASSERT_EQ(seg_cmap->cid(0x1240), 112u);
	ASSERT_EQ(seg_cmap->cid(0x1241), 0u);
	ASSERT_EQ(seg_cmap->cid(0x12FF), 0u);
	ASSERT_EQ(seg_cmap->cid(0x1300), 200u);
	ASSERT_EQ(seg_cmap->cid(0x1301), 201u);
	ASSERT_EQ(seg_cmap->cid(0x1302), 202u);
	ASSERT_EQ(seg_cmap->cid(0x1303), 0u);
}
