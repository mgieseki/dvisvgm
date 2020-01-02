/*************************************************************************
** TpicSpecialTest.cpp                                                  **
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
#include "SpecialActions.hpp"
#include "TpicSpecialHandler.hpp"
#include "utility.hpp"
#include "XMLNode.hpp"
#include "XMLString.hpp"

using namespace std;


class TpicSpecialTest : public ::testing::Test {
	protected:
		class ActionsRecorder : public EmptySpecialActions {
			public:
				ActionsRecorder () : x(), y() {}
				void embed (const BoundingBox &bb) override  {bbox.embed(bb);}
				void setX (double xx) override               {x = xx;}
				void setY (double yy) override               {x = yy;}
				double getX () const override                {return x;}
				double getY () const override                {return y;}
				Color getColor () const override             {return color;}
				void setColor (const Color &c) override      {color = c;}
				const Matrix& getMatrix () const override    {static Matrix m(1); return m;}

				string getXMLSnippet () const {
					ostringstream oss;
					for (XMLNode *child : *svgTree().pageNode())
						child->write(oss);
					return oss.str();
				}

				void clear () {
					SpecialActions::svgTree().reset();
					SpecialActions::svgTree().newPage(1);
					bbox = BoundingBox(0, 0, 0, 0);
				}

			private:
				double x, y;
				Color color;
				BoundingBox bbox;
		};


		class MyTpicSpecialHandler : public TpicSpecialHandler {
			public:
				explicit MyTpicSpecialHandler (SpecialActions &a) : actions(a) {}
				void finishPage () {dviEndPage(0, actions);}
				bool processSpecial (const string &cmd, string params="") {
					stringstream ss;
					ss << params;
					return process(cmd, ss, actions);
				}

			protected:
				SpecialActions &actions;
		};

	public:
		TpicSpecialTest () : handler(recorder) {}

		void SetUp () override {
			XMLString::DECIMAL_PLACES=2;
			recorder.clear();
		}


	protected:
		ActionsRecorder recorder;
		MyTpicSpecialHandler handler;
};


TEST_F(TpicSpecialTest, name) {
	EXPECT_EQ(handler.name(), "tpic");
}


TEST_F(TpicSpecialTest, fail) {
	EXPECT_FALSE(handler.processSpecial(""));
	EXPECT_FALSE(handler.processSpecial("x"));
	EXPECT_FALSE(handler.processSpecial("xy"));
	EXPECT_FALSE(handler.processSpecial("xyz"));
	EXPECT_FALSE(handler.processSpecial("bk "));
}


TEST_F(TpicSpecialTest, set_fill_intensity) {
	EXPECT_LT(handler.grayLevel(), 0);  // no fill color set by default
	EXPECT_TRUE(handler.processSpecial("bk"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	EXPECT_TRUE(handler.processSpecial("wh"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0);
	EXPECT_TRUE(handler.processSpecial("sh"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0.5);
	EXPECT_TRUE(handler.processSpecial("sh", "0.1"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0.1);
	EXPECT_TRUE(handler.processSpecial("sh", "0.6"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0.6);
	EXPECT_TRUE(handler.processSpecial("sh", "2"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	EXPECT_TRUE(handler.processSpecial("sh", "-2"));
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, set_penwidth) {
	const double mi2bp=0.072;  // milli-inch to PS points
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1);
	handler.processSpecial("pn", "1000");
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1000*mi2bp);
	handler.processSpecial("pn", "-10");
	EXPECT_DOUBLE_EQ(handler.penwidth(), 0);
}


TEST_F(TpicSpecialTest, dot) {
	handler.processSpecial("pn", "1000");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("bk");
	handler.processSpecial("fp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<circle cx='0' cy='0' r='36'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_polyline) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("fp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polyline fill='none' stroke-linecap='round' points='0,0 72,72 72,0' stroke='#000' stroke-width='1'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_polygon) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("fp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polygon fill='none' points='0,0 72,72 72,0' stroke='#000' stroke-width='1'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, fill_polygon) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("wh");
	handler.processSpecial("fp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polygon fill='#fff' points='0,0 72,72 72,0' stroke='#000' stroke-width='1'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
	recorder.clear();
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("wh");
	handler.processSpecial("ip");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polygon fill='#fff' points='0,0 72,72 72,0'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, fill_dashed_polygon) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("wh");
	handler.processSpecial("da", "2");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polygon fill='#fff' points='0,0 72,72 72,0' stroke='#000' stroke-width='1' stroke-dasharray='144'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, fill_dotted_polygon) {
	handler.processSpecial("pn", "500");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1000 0");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("wh");
	handler.processSpecial("dt", "2 2");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polygon fill='#fff' points='0,0 72,72 72,0' stroke='#000' stroke-width='36' stroke-dasharray='36 144'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_spline) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("sp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<polyline fill='none' stroke-linecap='round' points='0,0 72,72' stroke='#000' stroke-width='1'/>"
	);
	recorder.clear();
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1500 500");
	handler.processSpecial("pa", "2000 1000");
	handler.processSpecial("pa", "3000 2000");
	handler.processSpecial("pa", "1000 500");
	handler.processSpecial("sp");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path fill='none' d='M0 0L36 36Q72 72 90 54T126 54T180 108T144 90L72 36' stroke='#000' stroke-width='1'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_dashed_spline) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1500 500");
	handler.processSpecial("pa", "2000 1000");
	handler.processSpecial("pa", "3000 2000");
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("sp", "1");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path fill='none' d='M0 0L36 36Q72 72 90 54T126 54T180 108T108 72Z' stroke='#000' stroke-width='1' stroke-dasharray='72'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_dotted_spline) {
	handler.processSpecial("pa", "0 0");
	handler.processSpecial("pa", "1000 1000");
	handler.processSpecial("pa", "1500 500");
	handler.processSpecial("pa", "2000 1000");
	handler.processSpecial("pa", "3000 2000");
	handler.processSpecial("pa", "1000 500");
	handler.processSpecial("sp", "-1");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path fill='none' d='M0 0L36 36Q72 72 90 54T126 54T180 108T144 90L72 36' stroke='#000' stroke-width='1' stroke-dasharray='1 72'/>"
	);
	EXPECT_DOUBLE_EQ(handler.penwidth(), 1.0);
	EXPECT_LT(handler.grayLevel(), 0);
}


TEST_F(TpicSpecialTest, stroke_ellipse) {
	handler.processSpecial("ar", "0 0 500 500 0 7");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<circle cx='0' cy='0' r='36' stroke-width='1' stroke='#000' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 7");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<ellipse cx='0' cy='0' rx='72' ry='36' stroke-width='1' stroke='#000' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("pn", "100");
	handler.processSpecial("ar", "0 0 1000 500 -1 -8");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<ellipse cx='0' cy='0' rx='72' ry='36' stroke-width='7.2' stroke='#000' fill='none'/>"
	);
}


TEST_F(TpicSpecialTest, fill_ellipse) {
	handler.processSpecial("bk");
	handler.processSpecial("ia", "0 0 500 500 0 7");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<circle cx='0' cy='0' r='36' fill='#000'/>"
	);
	recorder.clear();
	handler.processSpecial("bk");
	handler.processSpecial("ia", "0 0 1000 500 0 7");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<ellipse cx='0' cy='0' rx='72' ry='36' fill='#000'/>"
	);
	recorder.clear();
	handler.processSpecial("pn", "100");
	handler.processSpecial("wh");
	handler.processSpecial("ia", "0 0 1000 500 -1 -8");
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<ellipse cx='0' cy='0' rx='72' ry='36' fill='#fff'/>"
	);
}


TEST_F(TpicSpecialTest, stroke_arc) {
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(math::PI/4));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 0 1 50.91 25.46' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(math::PI/2));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 0 1 0 36' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(3*math::PI/4));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 0 1-50.91 25.46' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(math::PI));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 1 1-72 0' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(5*math::PI/4));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 1 1-50.91-25.46' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(3*math::PI/2));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 1 1 0-36' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
	recorder.clear();
	handler.processSpecial("ar", "0 0 1000 500 0 "+to_string(-3*math::PI/2));
	EXPECT_EQ(recorder.getXMLSnippet(),
		"<path d='M72 0A72 36 0 0 1 0 36' stroke-width='1' stroke='#000' stroke-linecap='round' fill='none'/>"
	);
}


TEST_F(TpicSpecialTest, bit_pattern) {
	handler.processSpecial("tx", "");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	handler.processSpecial("tx", " \t   \n  ");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	handler.processSpecial("tx", "INVALID");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	handler.processSpecial("tx", "0");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1);
	handler.processSpecial("tx", "f");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 0);
	handler.processSpecial("tx", "1248");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-4.0/16.0);
	handler.processSpecial("tx", "12480");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-4.0/20.0);
	handler.processSpecial("tx", "DEAD BEEF");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-24.0/32.0);
	handler.processSpecial("tx", "123456789abcdef");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-32.0/60.0);
	handler.processSpecial("tx", "123456789ABCDEF0");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-32.0/64.0);
	handler.processSpecial("tx", "1234 5678 9abc def ");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-32.0/60.0);
	handler.processSpecial("tx", "1234 5678 X 9abc def");
	EXPECT_DOUBLE_EQ(handler.grayLevel(), 1.0-13.0/32.0);
}
