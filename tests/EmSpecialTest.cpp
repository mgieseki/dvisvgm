/*************************************************************************
** EmSpecialTest.cpp                                                    **
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
#include "EmSpecialHandler.hpp"
#include "SpecialActions.hpp"
#include "XMLNode.hpp"
#include "XMLString.hpp"
#include "SVGTree.hpp"

using namespace std;


class EmSpecialTest : public ::testing::Test {
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
				string getPageXML () const                   {ostringstream oss; oss << *svgTree().pageNode(); return oss.str();}
				const Matrix& getMatrix () const override    {static Matrix m(1); return m;}

				void clear () {
					SpecialActions::svgTree().reset();
					SpecialActions::svgTree().newPage(1);
					bbox = BoundingBox(0, 0, 0, 0);
				}

/*				void write (ostream &os) const {
					os << "page: " << page << '\n'
						<< "bbox: " << bbox.toSVGViewBox() << '\n';
				} */

			private:
				double x, y;
				BoundingBox bbox;
				Color color;
		};


		class MyEmSpecialHandler : public EmSpecialHandler {
			public:
				explicit MyEmSpecialHandler (SpecialActions &a) : actions(a) {}
				void finishPage () {dviEndPage(0, actions);}
				void processSpecial (const string &str) {stringstream ss;	ss << str; process("em", ss, actions);}

			protected:
				SpecialActions &actions;
		};

	public:
		EmSpecialTest () : handler(recorder) {}

		void SetUp () override {
			XMLString::DECIMAL_PLACES=2;
			recorder.clear();
		}


	protected:
		ActionsRecorder recorder;
		MyEmSpecialHandler handler;
};


TEST_F(EmSpecialTest, basic) {
	EXPECT_EQ(handler.name(), "em");
}


TEST_F(EmSpecialTest, lines1) {
	DPair p[] = {DPair(0,0), DPair(0,10), DPair(10,10), DPair(10,0)};
	int n = sizeof(p)/sizeof(DPair);
	for (int i=0; i < n; i++) {
		recorder.setX(p[i].x());
		recorder.setY(p[i].y());
		handler.processSpecial(string("point ")+XMLString(i));
	}
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'/>");
	handler.processSpecial("linewidth 2bp");
	for (int i=0; i < n; i++)
		handler.processSpecial(string("line ")+XMLString(i)+", "+XMLString((i+1)%n));
	EXPECT_EQ(recorder.getPageXML(),
		"<g id='page1'>\n"
		"<line x1='0' y1='0' x2='10' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='10' y1='0' x2='10' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='10' y1='0' x2='0' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='0' y1='0' x2='0' y2='0' stroke-width='2' stroke='#000'/>\n"
		"</g>"
	);
}


TEST_F(EmSpecialTest, lines2) {
	DPair p[] = {DPair(0,0), DPair(0,10), DPair(10,10), DPair(10,0)};
	int n = sizeof(p)/sizeof(DPair);
	handler.processSpecial("linewidth 2bp");
	// define lines prior to the points
	for (int i=0; i < n; i++)
		handler.processSpecial(string("line ")+XMLString(i)+", "+XMLString((i+1)%n));
	// define points
	for (int i=0; i < n; i++) {
		recorder.setX(p[i].x());
		recorder.setY(p[i].y());
		handler.processSpecial(string("point ")+XMLString(i));
	}
	handler.finishPage();
	EXPECT_EQ(recorder.getPageXML(),
		"<g id='page1'>\n"
		"<line x1='0' y1='0' x2='10' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='10' y1='0' x2='10' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='10' y1='0' x2='0' y2='0' stroke-width='2' stroke='#000'/>\n"
		"<line x1='0' y1='0' x2='0' y2='0' stroke-width='2' stroke='#000'/>\n"
		"</g>"
	);
}


TEST_F(EmSpecialTest, pline) {
	handler.processSpecial("point 1, 10, 10");
	handler.processSpecial("point 2, 100, 100");
	handler.processSpecial("line 1, 2, 10bp");
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'>\n<line x1='10' y1='10' x2='100' y2='100' stroke-width='10' stroke='#000'/>\n</g>");
}


TEST_F(EmSpecialTest, vline) {
	handler.processSpecial("point 1, 10, 10");
	handler.processSpecial("point 2, 100, 100");
	handler.processSpecial("line 1v, 2v, 10bp");  // cut line ends vertically
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'>\n<polygon points='10,17.07 10,2.93 100,92.93 100,107.07'/>\n</g>");
}


TEST_F(EmSpecialTest, hline) {
	handler.processSpecial("point 1, 10, 10");
	handler.processSpecial("point 2, 100, 100");
	handler.processSpecial("line 1h, 2h, 10bp");  // cut line ends horizontally
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'>\n<polygon points='2.93,10 17.07,10 107.07,100 92.93,100'/>\n</g>");
}


TEST_F(EmSpecialTest, hvline) {
	handler.processSpecial("point 1, 10, 10");
	handler.processSpecial("point 2, 100, 100");
	handler.processSpecial("line 1h, 2v, 10bp");  // cut line ends horizontally
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'>\n<polygon points='2.93,10 17.07,10 100,92.93 100,107.07'/>\n</g>");

	recorder.clear();
	recorder.setColor(Color(0.0, 0.0, 1.0));
	handler.processSpecial("point 1, 10, 10");
	handler.processSpecial("point 2, 100, 100");
	handler.processSpecial("line 1v, 2h, 10bp");  // cut line ends horizontally
	EXPECT_EQ(recorder.getPageXML(), "<g id='page1'>\n<polygon points='10,17.07 10,2.93 107.07,100 92.93,100' fill='#00f'/>\n</g>");
}


TEST_F(EmSpecialTest, lineto) {
	DPair p[] = {DPair(0,0), DPair(0,10), DPair(10,10), DPair(10,0)};
	int n = sizeof(p)/sizeof(DPair);
	recorder.setColor(Color(1.0, 0.0, 0.0));
	for (int i=0; i <= n; i++) {
		recorder.setX(p[i%n].x());
		recorder.setY(p[i%n].y());
		handler.processSpecial("linewidth "+XMLString(2*i)+"bp");
		handler.processSpecial(i == 0 ? "moveto" : "lineto");
	}
	EXPECT_EQ(recorder.getPageXML(),
		"<g id='page1'>\n"
		"<line x1='0' y1='0' x2='10' y2='0' stroke-width='2' stroke='#f00'/>\n"
		"<line x1='10' y1='0' x2='10' y2='0' stroke-width='4' stroke='#f00'/>\n"
		"<line x1='10' y1='0' x2='0' y2='0' stroke-width='6' stroke='#f00'/>\n"
		"<line x1='0' y1='0' x2='0' y2='0' stroke-width='8' stroke='#f00'/>\n"
		"</g>"
	);
}

