/*************************************************************************
** ColorTest.cpp                                                        **
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
#include "Color.hpp"

using namespace std;

TEST(ColorTest, construct) {
	EXPECT_EQ(Color(uint8_t(1), uint8_t(2), uint8_t(3)).rgbString(), "#010203");
	EXPECT_EQ(Color(1.0/255, 2.0/255, 3.0/255).rgbString(), "#010203");
	EXPECT_EQ(Color("Blue").rgbString(), "#00f");
	EXPECT_EQ(Color("blue").rgbString(), "#00f");
	EXPECT_EQ(Color("invalid").rgbString(), "#000");
}


TEST(ColorTest, gray) {
	Color c;
	c.setGray(10.0/255);
	EXPECT_EQ(c.rgbString(), "#0a0a0a");
	c.setGray(uint8_t(15));
	EXPECT_EQ(c.rgbString(), "#0f0f0f");
}


TEST(ColorTest, hsb) {
	Color c;
	c.setHSB(20.0/360, 0.75, 0.8);        // i==0
	EXPECT_EQ(c.rgbString(), "#c63");
	c.setHSB(60.0/360, 0.75, 0.8);        // i==1
	EXPECT_EQ(c.rgbString(), "#cc3");
	c.setHSB(120.0/360, 1.0, 1.0);        // i==2
	EXPECT_EQ(c.rgbString(), "#0f0");
	c.setHSB(180.0/360, 1.0, 1.0);        // i==3
	EXPECT_EQ(c.rgbString(), "#0ff");
	c.setHSB(240.0/360, 0.75, 0.8);       // i==4
	EXPECT_EQ(c.rgbString(), "#33c");
	c.setHSB(300.0/360, 0.75, 0.8);       // i==5
	EXPECT_EQ(c.rgbString(), "#c3c");
	c.setHSB(0.0, 1.0, 1.0);
	EXPECT_EQ(c.rgbString(), "#f00");
	c.setHSB(20.0/360, 0, 0.2);           // s==0
	EXPECT_EQ(c.rgbString(), "#333");
}


TEST(ColorTest, cmyk) {
	Color color;
	color.setCMYK(0.5, 0.4, 0.6, 0.2);
	EXPECT_EQ(color.rgbString(), "#667a52");
	double c, m, y, k;
	color.setRGB(0.0, 0.0, 0.0);
	color.getCMYK(c, m, y, k);
	EXPECT_DOUBLE_EQ(c, 0.0);
	EXPECT_DOUBLE_EQ(m, 0.0);
	EXPECT_DOUBLE_EQ(y, 0.0);
	EXPECT_DOUBLE_EQ(k, 1.0);
	color.setRGB(1.0, 0.0, 0.0);
	color.getCMYK(c, m, y, k);
	EXPECT_DOUBLE_EQ(c, 0.0);
	EXPECT_DOUBLE_EQ(m, 1.0);
	EXPECT_DOUBLE_EQ(y, 1.0);
	EXPECT_DOUBLE_EQ(k, 0.0);
}


TEST(ColorTest, name) {
	Color c;
	EXPECT_TRUE(c.setPSName("Magenta"));
	EXPECT_EQ(c.rgbString(), "#f0f");
	EXPECT_FALSE(c.setPSName("mulberry"));
	EXPECT_TRUE(c.setPSName("mulberry", false));
	EXPECT_EQ(c.rgbString(), "#a314f9");

	EXPECT_FALSE(c.setPSName("#Magenta"));
	EXPECT_FALSE(c.setPSName("#bcdefg"));
	EXPECT_TRUE(c.setPSName("#abcdef"));
	EXPECT_EQ(c.rgbString(), "#abcdef");
	EXPECT_TRUE(c.setPSName("#89A"));
	EXPECT_EQ(c.rgbString(), "#00089a");
	EXPECT_TRUE(c.setPSName("#A98 \n "));
	EXPECT_EQ(c.rgbString(), "#000a98");
}


TEST(ColorTest, getXYZ) {
	Color c;
	double x, y, z;
	c.setRGB(1.0, 0, 0);
	c.getXYZ(x, y, z);
	EXPECT_NEAR(x, 0.412456, 0.000001);
	EXPECT_NEAR(y, 0.212673, 0.000001);
	EXPECT_NEAR(z, 0.019334, 0.000001);

	c.setRGB(0, 1.0, 0);
	c.getXYZ(x, y, z);
	EXPECT_NEAR(x, 0.357576, 0.000001);
	EXPECT_NEAR(y, 0.715152, 0.000001);
	EXPECT_NEAR(z, 0.119192, 0.000001);

	c.setRGB(0, 0, 1.0);
	c.getXYZ(x, y, z);
	EXPECT_NEAR(x, 0.180437, 0.000001);
	EXPECT_NEAR(y, 0.072175, 0.000001);
	EXPECT_NEAR(z, 0.950304, 0.000001);

	valarray<double> rgb(3), xyz(3);
	rgb[0] = 0.2;
	rgb[1] = 0.75;
	rgb[2] = 0.9;
	Color::RGB2XYZ(rgb, xyz);
	EXPECT_NEAR(xyz[0], 0.342574, 0.000001);
	EXPECT_NEAR(xyz[1], 0.437554, 0.000001);
	EXPECT_NEAR(xyz[2], 0.811202, 0.000001);
}


TEST(ColorTest, getLab) {
	Color c;
	double l, a, b;
	c.setRGB(1.0, 0, 0);
	c.getLab(l, a, b);
	EXPECT_NEAR(l, 53.2408, 0.0001);
	EXPECT_NEAR(a, 80.0925, 0.0001);
	EXPECT_NEAR(b, 67.2032, 0.0001);

	c.setRGB(0, 1.0, 0);
	c.getLab(l, a, b);
	EXPECT_NEAR(l, 87.7347, 0.0001);
	EXPECT_NEAR(a, -86.1827, 0.0001);
	EXPECT_NEAR(b, 83.1793, 0.0001);

	c.setRGB(0, 0, 1.0);
	c.getLab(l, a, b);
	EXPECT_NEAR(l, 32.2970, 0.0001);
	EXPECT_NEAR(a, 79.1875, 0.0001);
	EXPECT_NEAR(b, -107.8602, 0.0001);

	valarray<double> rgb(3), lab(3);
	rgb[0] = 0.2;
	rgb[1] = 0.75;
	rgb[2] = 0.9;
	Color::RGB2Lab(rgb, lab);
	EXPECT_NEAR(lab[0], 72.0647, 0.0001);
	EXPECT_NEAR(lab[1], -23.7597, 0.0001);
	EXPECT_NEAR(lab[2], -29.4733, 0.0001);
}


TEST(ColorTest, svgColorString) {
	EXPECT_EQ(Color(uint32_t(0x000000)).svgColorString(false), "black");
	EXPECT_EQ(Color(uint32_t(0xffffff)).svgColorString(false), "white");
	EXPECT_EQ(Color(uint32_t(0xff0000)).svgColorString(false), "red");
	EXPECT_EQ(Color(uint32_t(0x9400d3)).svgColorString(false), "darkviolet");
	EXPECT_EQ(Color(uint32_t(0x000001)).svgColorString(false), "#000001");

	EXPECT_EQ(Color(uint32_t(0x000000)).svgColorString(true), "#000");
	EXPECT_EQ(Color(uint32_t(0xffffff)).svgColorString(true), "#fff");
	EXPECT_EQ(Color(uint32_t(0xff0000)).svgColorString(true), "#f00");
	EXPECT_EQ(Color(uint32_t(0x9400d3)).svgColorString(true), "#9400d3");
	EXPECT_EQ(Color(uint32_t(0x000001)).svgColorString(true), "#000001");
}


TEST(ColorTest, components) {
	EXPECT_EQ(Color::numComponents(Color::ColorSpace::GRAY), 1);
	EXPECT_EQ(Color::numComponents(Color::ColorSpace::RGB), 3);
	EXPECT_EQ(Color::numComponents(Color::ColorSpace::LAB), 3);
	EXPECT_EQ(Color::numComponents(Color::ColorSpace::CMYK), 4);
}


TEST(ColorTest, scale) {
	EXPECT_EQ((Color(uint32_t(0x123456)) *= 0.0).rgbString(), "#000");
	EXPECT_EQ((Color(uint32_t(0x123456)) *= 1.0).rgbString(), "#123456");
	EXPECT_EQ((Color(uint32_t(0x123456)) *= 3.0).rgbString(), "#369d02");
	EXPECT_EQ((Color(uint32_t(0x123456)) *= 0.5).rgbString(), "#091a2b");
}


TEST(ColorTest, set) {
	Color color;
	vector<double> vec{0.1, 0.2, 0.3, 0.4};
	VectorIterator<double> it(vec);
	color.set(Color::ColorSpace::GRAY, it);
	EXPECT_EQ(uint32_t(color), 0x1a1a1au);
	it.reset();
	color.set(Color::ColorSpace::RGB, it);
	EXPECT_EQ(uint32_t(color), 0x1a334du);
	it.reset();
	color.set(Color::ColorSpace::CMYK, it);
	EXPECT_EQ(uint32_t(color), 0x8a7a6bu);
	it.reset();
	color.set(Color::ColorSpace::LAB, it);
	EXPECT_EQ(uint32_t(color), 0x010000u);
}
