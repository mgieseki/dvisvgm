/*************************************************************************
** ShadingPatchTest.cpp                                                 **
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
#include "ShadingPatch.hpp"
#include "TensorProductPatch.hpp"
#include "TriangularPatch.hpp"

using namespace std;


TEST(ShadingPatchTest, create) {
	auto patch = ShadingPatch::create(4, Color::ColorSpace::RGB);
	EXPECT_TRUE(dynamic_cast<TriangularPatch*>(patch.get()));
	patch = ShadingPatch::create(5, Color::ColorSpace::RGB);
	EXPECT_TRUE(dynamic_cast<LatticeTriangularPatch*>(patch.get()));
	patch = ShadingPatch::create(6, Color::ColorSpace::RGB);
	EXPECT_TRUE(dynamic_cast<CoonsPatch*>(patch.get()));
	patch = ShadingPatch::create(7, Color::ColorSpace::RGB);
	EXPECT_TRUE(dynamic_cast<TensorProductPatch*>(patch.get()));
}


TEST(ShadingPatchTest, fail) {
	for (int i=0; i < 4; i++) {
		EXPECT_THROW(ShadingPatch::create(i, Color::ColorSpace::RGB), ShadingException);
	}
}


TEST(ShadingPatchTest, colorQueryFuncs) {
	vector<DPair> points(3);
	vector<Color> colors(3);
	colors[0].setRGB(1.0, 0.0, 0.0);
	colors[1].setRGB(0.0, 1.0, 0.0);
	colors[2].setRGB(0.0, 0.0, 1.0);
	TriangularPatch tp1(points, colors, Color::ColorSpace::RGB, 0, 0);
	EXPECT_EQ(tp1.colorAt(0, 0).rgbString(), "#f00");
	EXPECT_EQ(tp1.colorAt(1, 0).rgbString(), "#0f0");
	EXPECT_EQ(tp1.colorAt(0, 1).rgbString(), "#00f");

	colors[0].setGray(0.2);
	colors[1].setGray(0.4);
	colors[2].setGray(0.6);
	TriangularPatch tp2(points, colors, Color::ColorSpace::GRAY, 0, 0);
	EXPECT_EQ(tp2.colorAt(0, 0).rgbString(), "#333");
	EXPECT_EQ(tp2.colorAt(1, 0).rgbString(), "#666");
	EXPECT_EQ(tp2.colorAt(0, 1).rgbString(), "#999");

	colors[0].setCMYK(1.0, 0.0, 0.0, 0.2);
	colors[1].setCMYK(0.0, 1.0, 0.0, 0.4);
	colors[2].setCMYK(0.0, 0.0, 1.0, 0.6);
	TriangularPatch tp3(points, colors, Color::ColorSpace::CMYK, 0, 0);
	EXPECT_EQ(tp3.colorAt(0, 0).rgbString(), "#29cccc");
	EXPECT_EQ(tp3.colorAt(1, 0).rgbString(), "#993d99");
	EXPECT_EQ(tp3.colorAt(0, 1).rgbString(), "#66663d");

	colors[0].setLab(55, 80, 68);
	colors[1].setLab(30, 62, -108);
	colors[2].setLab(85, -72, 61);
	TriangularPatch tp4(points, colors, Color::ColorSpace::LAB, 0, 0);
	EXPECT_EQ(tp4.colorAt(0, 0).rgbString(), "#ff1402");
	EXPECT_EQ(tp4.colorAt(1, 0).rgbString(), "#002bf8");
	EXPECT_EQ(tp4.colorAt(0, 1).rgbString(), "#4af356");
}
