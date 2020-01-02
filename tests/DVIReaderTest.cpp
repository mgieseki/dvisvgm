/*************************************************************************
** DVIReaderTest.cpp                                                    **
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
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include "DVIReader.hpp"
#include "Font.hpp"
#include "Message.hpp"
#include "XMLString.hpp"

using namespace std;

template <typename T>
void write (ostream &os, T val) {
	os << val;
}

template<>
void write (ostream &os, double val) {
	os << XMLString(val);
}


template <typename T>
ostream& operator << (ostream &os, const vector<T> &vec) {
	os << '{';
	for (size_t i=0; i < vec.size(); i++) {
		if (i > 0)
			os << ", ";
		write(os, vec[i]);
	}
	os << '}';
	return os;
}


class LoggingDVIReader : public DVIReader {
	public:
		LoggingDVIReader (istream& is, ostream &os) : DVIReader (is), _inVirtualFont(false), _os(os) {}

		void execute () {
			clearStream();
			seek(0);
			executeAll();
		}

	protected:
		void dviPre (uint8_t id, uint32_t numer, uint32_t denom, uint32_t mag, const string &comment) override {
			_os << "pre " << int(id) << ", " << numer << ", " << denom << ", " <<  mag << ", '" << comment << "'";
		}

		void dviPost (uint16_t stdepth, uint16_t pages, uint32_t pw, uint32_t ph, uint32_t mag, uint32_t num, uint32_t den, uint32_t lbopofs) override {
			_os << "post " << stdepth << ", " << pages << ", " << pw << ", " << ph << ", " <<  mag << ", " <<  num << ", " << den << ", " <<  lbopofs;
		}

		void dviPostPost (uint8_t id, uint32_t postOffset) override {
			_os << "postpost " << int(id) << ", " << postOffset;
		}

		void dviBop (const vector<int32_t> &c, int32_t prevBopOffset) override {
			_os << "bop " << c << ", " << prevBopOffset;
		}

		void dviEop () override {_os << "eop";}
		void dviSetChar0 (uint32_t c, const Font *font) override {if (!_inVirtualFont) _os << "setchar0 " << c;}
		void dviSetChar (uint32_t c, const Font *font) override  {if (!_inVirtualFont) _os << "setchar " << c;}
		void dviPutChar (uint32_t c, const Font *font) override  {if (!_inVirtualFont) _os << "putchar " << c;}
		void dviSetRule (double h, double w) override {_os << "setrule " << XMLString(h) << ", " << XMLString(w);}
		void dviPutRule (double h, double w) override {_os << "putrule " << XMLString(h) << ", " << XMLString(w);}
		void dviNop () override  {_os << "nop";}
		void dviPush () override {_os << "push";}
		void dviPop () override  {_os << "pop";}
		void dviRight (double dx) override {_os << "right " << XMLString(dx);}
		void dviDown (double dy) override {_os << "down " << XMLString(dy);}
		void dviX0 () override {_os << "x0";}
		void dviY0 () override {_os << "y0";}
		void dviW0 () override {_os << "w0";}
		void dviZ0 () override {_os << "z0";}
		void dviX (double x) override {_os << "x " << XMLString(x);}
		void dviY (double y) override {_os << "y " << XMLString(y);}
		void dviW (double w) override {_os << "w " << XMLString(w);}
		void dviZ (double z) override {_os << "z " << XMLString(z);}
		void dviDir (WritingMode dir) override {_os << "dir " << int(dir);}
		void dviXXX (const string &str) override {_os << "xxx '" << str << "'";}

		void dviFontDef (uint32_t fontnum, uint32_t checksum, const Font *font) override {
			_os << "fontdef " << fontnum << ", " << checksum << ", " << font->name();
		}

		void dviFontNum (uint32_t fontnum, SetFontMode mode, const Font *font) override {
			if (!_inVirtualFont) { // exclude logging of virtual fonts
				if (mode == DVIReader::SetFontMode::VF_ENTER)
					_inVirtualFont = true;
				else
					_os << "fontnum " << fontnum << ", " << font->name();
			}
			else if (mode == DVIReader::SetFontMode::VF_LEAVE)
				_inVirtualFont = false;
		}

		void dviXPic (uint8_t box, const vector<int32_t> &matrix, int16_t page, const string &path) override {
			_os << "xpic " << int(box) << ", " << matrix << ", " << page << ", " << path;
		}

		void dviXFontDef (uint32_t fontnum, const NativeFont *font) override {
			_os << "xfontdef " << fontnum;
		}

		void dviXGlyphArray (vector<double> &dx, vector<double> &dy, vector<uint16_t> &glyphs, const Font &font) override {
			_os << "xglypharray " << dx << ", " << dy << ", " << glyphs << ", " << font.name();
		}

		void dviXGlyphString (vector<double> &dx, vector<uint16_t> &glyphs, const Font &font) override {
			_os << "xglyphstring " << dx << ", " << glyphs << ", " << font.name();
		}

		void dviXTextAndGlyphs (vector<double> &dx, vector<double> &dy, vector<uint16_t> &chars, vector<uint16_t> &glyphs, const Font &font) override {
			_os << "xtxtglyphs " << dx << ", " << dy << ", " << chars << ", " << glyphs << ", " << font.name();
		}

		int executeCommand () override {
			int ret = DVIReader::executeCommand();
			if (!_inVirtualFont) {
				_os << " [h=" << XMLString(dviState().h)
					<< ", v=" << XMLString(dviState().v)
					<< ", x=" << XMLString(dviState().x)
					<< ", y=" << XMLString(dviState().y)
					<< ", w=" << XMLString(dviState().w)
					<< ", z=" << XMLString(dviState().z)
					<< ", d=" << int(dviState().d)
					<< "]\n";
			}
			return ret;
		}

	private:
		bool _inVirtualFont;  ///< true if reading a char definition of a virtual font
		ostream &_os;
};


static void test_dvi_file (const char *fname, const vector<const char*> &expected_lines) {
	XMLString::DECIMAL_PLACES = 3;
	string dviname = string(SRCDIR)+"/data/"+fname;
	ifstream ifs(dviname, ios::binary);
	ASSERT_TRUE(bool(ifs));
	stringstream ss;
	LoggingDVIReader dviReader(ifs, ss);
	dviReader.execute();
	ss.seekg(0);
	int lineno=0;
	char line[512];
	for (const string expected_line : expected_lines) {
		ASSERT_FALSE(ss.eof());
		ss.getline(line, 512);
		lineno++;
		EXPECT_EQ(line, expected_line) << "log line #" << lineno;
	}
	ss.getline(line, 512);
	EXPECT_EQ(*line, '\0');
	EXPECT_TRUE(ss.eof());
}


// test DVI file generated with standard LaTeX (DVI version 2)
TEST(DVIReaderTest, dvi_v2) {
	vector<const char*> expected_lines {
		"pre 2, 25400000, 473628672, 1000, ' TeX output 2017.11.22:1132' [h=0, v=0, x=0, y=0, w=0, z=0, d=0]",
		"bop {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, -1 [h=0, v=0, x=0, y=0, w=0, z=0, d=0]",
		"down 630.635 [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"down -602.74 [h=0, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"right 61.768 [h=61.768, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"xxx 'color push gray 0' [h=61.768, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"right 343.711 [h=405.479, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"xxx 'color pop' [h=405.479, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=27.895, x=0, y=0, w=0, z=0, d=0]",
		"down 572.852 [h=0, v=600.747, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=600.747, x=0, y=0, w=0, z=0, d=0]",
		"right 61.768 [h=61.768, v=600.747, x=0, y=0, w=0, z=0, d=0]",
		"down -547.945 [h=61.768, v=52.802, x=0, y=0, w=0, z=0, d=0]",
		"xxx 'color push gray 0' [h=61.768, v=52.802, x=0, y=0, w=0, z=0, d=0]",
		"down 9.963 [h=61.768, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"fontdef 7, 1274110073, cmr10 [h=61.768, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"fontnum 7, cmr10 [h=61.768, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 67 [h=68.964, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 111 [h=73.945, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 108 [h=76.712, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 111 [h=81.694, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 114 [h=85.596, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"w 3.321 [h=88.917, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=92.791, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=97.219, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 115 [h=101.149, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=105.023, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 58 [h=107.79, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"right 4.428 [h=112.218, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color push rgb 1 0 0' [h=112.218, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 114 [h=116.12, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=120.548, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 100 [h=126.083, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color pop' [h=126.083, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 44 [h=128.85, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"w0 [h=132.171, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color push rgb 0 0 1' [h=132.171, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 98 [h=137.706, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 108 [h=140.473, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 117 [h=146.008, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=150.436, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color pop' [h=150.436, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 44 [h=153.204, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"w0 [h=156.524, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color push cmyk 0 1 0 0' [h=156.524, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 109 [h=164.827, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 97 [h=169.808, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 103 [h=174.789, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=179.217, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 110 [h=184.752, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"right -.277 [h=184.475, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=188.35, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 97 [h=193.331, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'color pop' [h=193.331, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 46 [h=196.098, v=62.765, x=0, y=0, w=3.321, z=0, d=0]",
		"pop [h=61.768, v=62.765, x=0, y=0, w=0, z=0, d=0]",
		"down 44.638 [h=61.768, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 76 [h=67.995, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 105 [h=70.762, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 110 [h=76.297, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 101 [h=80.725, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"w 3.321 [h=84.046, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 111 [h=89.027, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 102 [h=92.071, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"w0 [h=95.392, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=99.267, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=103.695, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 120 [h=108.953, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=112.827, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"w0 [h=116.148, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 119 [h=123.343, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 105 [h=126.111, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=129.985, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 104 [h=135.52, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"w0 [h=138.841, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 97 [h=143.822, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"push [h=143.822, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"push [h=143.822, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"right 6.78 [h=150.602, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'ps: gsave currentpoint currentpoint translate 30 neg rotate neg exch neg exch translate' [h=150.602, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"push [h=150.602, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 114 [h=154.504, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 111 [h=159.485, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=163.36, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 97 [h=168.341, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 116 [h=172.215, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 101 [h=176.643, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 100 [h=182.178, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"pop [h=150.602, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"right 31.576 [h=182.178, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"xxx 'ps: currentpoint grestore moveto' [h=182.178, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"pop [h=143.822, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"pop [h=143.822, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"right 37.446 [h=181.268, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 119 [h=188.463, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"right -.277 [h=188.187, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 111 [h=193.168, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 114 [h=197.07, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 100 [h=202.605, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"setchar0 46 [h=205.372, v=107.402, x=0, y=0, w=3.321, z=0, d=0]",
		"pop [h=61.768, v=107.402, x=0, y=0, w=0, z=0, d=0]",
		"down 52.873 [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"down -31.154 [h=61.768, v=129.121, x=0, y=0, w=0, z=0, d=0]",
		"putrule .797, 220.922 [h=61.768, v=129.121, x=0, y=0, w=0, z=0, d=0]",
		"y 11.157 [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"right 5.978 [h=67.746, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 12 [h=73.281, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 114 [h=77.183, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 115 [h=81.113, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 116 [h=84.987, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"right 3.321 [h=88.308, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 99 [h=92.736, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=97.717, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 108 [h=100.484, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 117 [h=106.019, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 109 [h=114.321, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 110 [h=119.856, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"pop [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"right 110.558 [h=172.326, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 115 [h=176.256, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 101 [h=180.684, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 99 [h=185.112, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=190.093, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 110 [h=195.628, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 100 [h=201.163, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"right 3.321 [h=204.483, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 99 [h=208.911, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=213.893, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 108 [h=216.66, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 117 [h=222.195, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 109 [h=230.497, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 110 [h=236.032, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"pop [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"pop [h=61.768, v=140.278, x=0, y=11.157, w=0, z=0, d=0]",
		"down 5.8 [h=61.768, v=146.078, x=0, y=11.157, w=0, z=0, d=0]",
		"putrule .498, 220.922 [h=61.768, v=146.078, x=0, y=11.157, w=0, z=0, d=0]",
		"y0 [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"right 5.978 [h=67.746, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 76 [h=73.973, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=78.954, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 114 [h=82.856, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 101 [h=87.284, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 109 [h=95.586, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"right 3.321 [h=98.907, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 105 [h=101.674, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 112 [h=107.209, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 115 [h=111.139, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 117 [h=116.674, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 109 [h=124.976, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"pop [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"push [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"right 110.558 [h=172.326, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 100 [h=177.861, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=182.842, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 108 [h=185.61, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 111 [h=190.591, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"setchar0 114 [h=194.493, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"w 3.321 [h=197.814, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 115 [h=201.744, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 105 [h=204.511, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 116 [h=208.386, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"w0 [h=211.706, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 97 [h=216.688, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 109 [h=224.99, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 101 [h=229.418, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"setchar0 116 [h=233.292, v=157.235, x=0, y=11.157, w=3.321, z=0, d=0]",
		"pop [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"pop [h=61.768, v=157.235, x=0, y=11.157, w=0, z=0, d=0]",
		"y 11.955 [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"right 5.978 [h=67.746, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 99 [h=72.174, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 111 [h=77.155, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 110 [h=82.69, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 115 [h=86.62, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 101 [h=91.048, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 116 [h=94.922, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 101 [h=99.35, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 116 [h=103.224, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 117 [h=108.759, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 114 [h=112.661, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"right 3.321 [h=115.982, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 115 [h=119.912, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 97 [h=124.893, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 100 [h=130.428, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 105 [h=133.195, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 112 [h=138.73, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 115 [h=142.66, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 99 [h=147.088, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 105 [h=149.855, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 110 [h=155.39, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 103 [h=160.371, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"pop [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"right 110.558 [h=172.326, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 101 [h=176.754, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 108 [h=179.521, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 105 [h=182.289, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 116 [h=186.163, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 114 [h=190.065, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"pop [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"pop [h=61.768, v=169.19, x=0, y=11.955, w=0, z=0, d=0]",
		"y0 [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"right 5.978 [h=67.746, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 115 [h=71.676, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 101 [h=76.104, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 100 [h=81.638, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"w 3.321 [h=84.959, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 100 [h=90.494, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 105 [h=93.261, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 97 [h=98.243, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 109 [h=106.545, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"w0 [h=109.866, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 110 [h=115.401, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 111 [h=120.382, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"setchar0 110 [h=125.917, v=181.145, x=0, y=11.955, w=3.321, z=0, d=0]",
		"w -.277 [h=125.64, v=181.145, x=0, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 117 [h=131.175, v=181.145, x=0, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 109 [h=139.477, v=181.145, x=0, y=11.955, w=-.277, z=0, d=0]",
		"w0 [h=139.2, v=181.145, x=0, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 121 [h=144.458, v=181.145, x=0, y=11.955, w=-.277, z=0, d=0]",
		"pop [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"push [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"right 110.558 [h=172.326, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 101 [h=176.754, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 105 [h=179.521, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 114 [h=183.423, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 109 [h=191.726, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"setchar0 111 [h=196.707, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"w .277 [h=196.984, v=181.145, x=0, y=11.955, w=.277, z=0, d=0]",
		"setchar0 100 [h=202.519, v=181.145, x=0, y=11.955, w=.277, z=0, d=0]",
		"x 3.321 [h=205.84, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 116 [h=209.714, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 101 [h=214.142, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 109 [h=222.444, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 112 [h=227.979, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"w0 [h=228.256, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 111 [h=233.237, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 114 [h=237.139, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"x0 [h=240.46, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 105 [h=243.227, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"setchar0 110 [h=248.762, v=181.145, x=3.321, y=11.955, w=.277, z=0, d=0]",
		"w -.277 [h=248.485, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 118 [h=253.743, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 105 [h=256.511, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 100 [h=262.046, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 117 [h=267.58, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 110 [h=273.115, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"w0 [h=272.838, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"setchar0 116 [h=276.713, v=181.145, x=3.321, y=11.955, w=-.277, z=0, d=0]",
		"pop [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"pop [h=61.768, v=181.145, x=0, y=11.955, w=0, z=0, d=0]",
		"down 6.099 [h=61.768, v=187.245, x=0, y=11.955, w=0, z=0, d=0]",
		"putrule .797, 220.922 [h=61.768, v=187.245, x=0, y=11.955, w=0, z=0, d=0]",
		"pop [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=160.275, x=0, y=0, w=0, z=0, d=0]",
		"down 58.197 [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"down 2.989 [h=61.768, v=221.461, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=221.461, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=221.461, x=0, y=0, w=0, z=0, d=0]",
		"down -2.989 [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"right 3.387 [h=65.156, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 98 [h=70.69, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"right .277 [h=70.967, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 111 [h=75.949, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"right -.277 [h=75.672, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 120 [h=80.93, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 101 [h=85.358, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 100 [h=90.893, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"right 3.321 [h=94.213, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 116 [h=98.088, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 101 [h=102.516, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 120 [h=107.774, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 116 [h=111.648, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=221.461, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=221.461, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"down -9.907 [h=61.768, v=208.565, x=0, y=0, w=0, z=0, d=0]",
		"putrule .398, 53.267 [h=61.768, v=208.565, x=0, y=0, w=0, z=0, d=0]",
		"down 13.095 [h=61.768, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"setrule 13.295, .398 [h=62.167, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"right 52.47 [h=114.637, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"setrule 13.295, .398 [h=115.035, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=221.66, x=0, y=0, w=0, z=0, d=0]",
		"down .199 [h=61.768, v=221.859, x=0, y=0, w=0, z=0, d=0]",
		"putrule .398, 53.267 [h=61.768, v=221.859, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=218.472, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=600.747, x=0, y=0, w=0, z=0, d=0]",
		"down 29.888 [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"right 61.768 [h=61.768, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"xxx 'color push gray 0' [h=61.768, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"push [h=61.768, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"right 169.365 [h=231.133, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 49 [h=236.115, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=61.768, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"right 343.711 [h=405.479, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"xxx 'color pop' [h=405.479, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"eop [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"post 10, 1, 405, 630, 1000, 25400000, 473628672, 42 [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"fontdef 7, 1274110073, cmr10 [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
		"postpost 2, 953 [h=0, v=630.635, x=0, y=0, w=0, z=0, d=0]",
	};
	test_dvi_file("sample_v2.dvi", expected_lines);
}


// test DVI file generated with pLaTeX in vertical mode (DVI version 3)
TEST(DVIReaderTest, dvi_v3) {
	vector<const char*> expected_lines {
		"pre 2, 25400000, 473628672, 1000, ' TeX output 2017.11.22:1928' [h=0, v=0, x=0, y=0, w=0, z=0, d=0]",
		"bop {1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, -1 [h=0, v=0, x=0, y=0, w=0, z=0, d=0]",
		"down 703.125 [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"down -678.472 [h=0, v=24.653, x=0, y=0, w=0, z=0, d=0]",
		"down 650.12 [h=0, v=674.773, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=674.773, x=0, y=0, w=0, z=0, d=0]",
		"down -601.603 [h=0, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"right 12.552 [h=12.552, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"down -30.128 [h=12.552, v=43.042, x=0, y=0, w=0, z=0, d=0]",
		"dir 1 [h=12.552, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"down -15.94 [h=28.492, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"push [h=28.492, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"down 3.404 [h=25.088, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"fontdef 7, 1274110073, cmr10 [h=25.088, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"fontnum 7, cmr10 [h=25.088, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 118 [h=25.088, v=48.3, x=0, y=0, w=0, z=0, d=1]",
		"right -.277 [h=25.088, v=48.023, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 101 [h=25.088, v=52.451, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 114 [h=25.088, v=56.353, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 116 [h=25.088, v=60.228, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 105 [h=25.088, v=62.995, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 99 [h=25.088, v=67.423, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 97 [h=25.088, v=72.404, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 108 [h=25.088, v=75.172, x=0, y=0, w=0, z=0, d=1]",
		"pop [h=28.492, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"down 15.94 [h=12.552, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"push [h=12.552, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"down 3.404 [h=9.148, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 116 [h=9.148, v=46.916, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 101 [h=9.148, v=51.344, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 120 [h=9.148, v=56.602, x=0, y=0, w=0, z=0, d=1]",
		"setchar0 116 [h=9.148, v=60.477, x=0, y=0, w=0, z=0, d=1]",
		"pop [h=12.552, v=43.042, x=0, y=0, w=0, z=0, d=1]",
		"pop [h=0, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=73.17, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=674.773, x=0, y=0, w=0, z=0, d=0]",
		"down 28.352 [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"push [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"right 223.147 [h=223.147, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"setchar0 49 [h=228.128, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"pop [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"eop [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"post 5, 1, 451, 703, 1000, 25400000, 473628672, 42 [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"fontdef 7, 1274110073, cmr10 [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
		"postpost 3, 195 [h=0, v=703.125, x=0, y=0, w=0, z=0, d=0]",
	};
	test_dvi_file("sample_v3.dvi", expected_lines);
}
