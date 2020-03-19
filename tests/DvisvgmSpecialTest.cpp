/*************************************************************************
** DvisvgmSpecialTest.cpp                                               **
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
#include <array>
#include <sstream>
#include "DvisvgmSpecialHandler.hpp"
#include "SpecialActions.hpp"
#include "XMLNode.hpp"

using namespace std;


class MyDvisvgmSpecialHandler : public DvisvgmSpecialHandler {
	public:
		void finishPreprocessing () {dviPreprocessingFinished();}
		void finishPage ()          {dviEndPage(0, _actions);}

	protected:
		EmptySpecialActions _actions;
};


class DvisvgmSpecialTest : public ::testing::Test {
	protected:
		class ActionsRecorder : public EmptySpecialActions {
			public:
				void embed (const BoundingBox &bb) override   {bbox.embed(bb);}
				double getX () const override                 {return -42;}
				double getY () const override                 {return 14;}
				bool defsEquals (const string &str) const     {return defsString() == str;}
				bool pageEquals (const string &str) const     {return pageString() == str;}
				bool bboxEquals (const string &str) const     {return bbox.toSVGViewBox() == str;}
				string bboxString () const                    {return bbox.toSVGViewBox();}
				string defsString () const                    {return toString(svgTree().defsNode());}
				string pageString () const                    {return toString(svgTree().pageNode());}

				void clear () {
					SpecialActions::svgTree().reset();
					SpecialActions::svgTree().newPage(1);
					bbox = BoundingBox(0, 0, 0, 0);
				}

			protected:
				string toString (const XMLNode *node) const {
					ostringstream oss;
					if (node)
						node->write(oss);
					return oss.str();
				}

			private:
				BoundingBox bbox;
		};

		void SetUp () override {
			recorder.clear();
			XMLElement::WRITE_NEWLINES = false;
		}

	protected:
		MyDvisvgmSpecialHandler handler;
		ActionsRecorder recorder;
};


TEST_F(DvisvgmSpecialTest, basic) {
	EXPECT_EQ(handler.name(), "dvisvgm");
}


TEST_F(DvisvgmSpecialTest, rawText) {
	istringstream iss("raw first{?nl}{?x},{?y}");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'>first\n-42,14</g>")) << recorder.pageString();

	iss.clear(); iss.str("raw \t ;{?(-x+2*y-5)}second {?bbox dummy} \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'>first\n-42,14;65second 0 0 0 0</g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPage1) {
	istringstream iss("raw <elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPage2) {
	istringstream iss("raw <elem attr1='1' attr2='20'>text1");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw text2<inner>text3</inner><my:empty-elem/>text4");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw </elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><elem attr1='1' attr2='20'>text1text2<inner>text3</inner><my:empty-elem/>text4</elem></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPage3) {
	istringstream iss("raw <elem attr1='1' attr2=");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw '20'>text2<inner>text3</inner>text4</e");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw lem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><elem attr1='1' attr2='20'>text2<inner>text3</inner>text4</elem></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawDefs1) {
	istringstream iss("rawdef <elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("<defs><elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem></defs>")) << recorder.defsString();
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
}


TEST_F(DvisvgmSpecialTest, rawDefs2) {
	istringstream iss("rawdef <elem attr1='1' attr2='20'>text1");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("rawdef text2<inner>text3</inner>text4");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("rawdef </elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("<defs><elem attr1='1' attr2='20'>text1text2<inner>text3</inner>text4</elem></defs>"));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
}


TEST_F(DvisvgmSpecialTest, rawDefs3) {
	istringstream iss("rawdef <elem attr1='1' a");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("rawdef ttr2='20'>text2<inner>text3</in");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("rawdef ner>text4</elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("<defs><elem attr1='1' attr2='20'>text2<inner>text3</inner>text4</elem></defs>"));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
}


TEST_F(DvisvgmSpecialTest, rawCDATA) {
	istringstream iss("raw <outer>text1<![CDATA[1 < 2 <!--test-->]]>text2</outer>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><outer>text1<![CDATA[1 < 2 <!--test-->]]>text2</outer></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawComments) {
	istringstream iss("raw <first/><second><!-- 1 < 2 ->--->text</second>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><first/><second><!-- 1 < 2 ->--->text</second></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPI) {
	istringstream iss("raw <first/><?pi1 whatever?><second><?pi2 whatever?></second>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><first/><?pi1 whatever?><second><?pi2 whatever?></second></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPageFail) {
	istringstream iss("raw <elem attr1='1' attr2='20'");
	EXPECT_THROW({handler.process("", iss, recorder); handler.finishPage();}, SpecialException);  // incomplete opening tag
	iss.clear(); iss.str("raw </elem>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // spurious closing tag
	iss.clear(); iss.str("raw <open>text</close>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // mismatching tags
}


TEST_F(DvisvgmSpecialTest, rawDefsFail) {
	istringstream iss("rawdef <elem attr1='1' attr2='20'");
	EXPECT_THROW({handler.process("", iss, recorder); handler.finishPage();}, SpecialException);  // incomplete opening tag
	iss.clear(); iss.str("rawdef </elem>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // spurious closing tag
	iss.clear(); iss.str("rawdef <open>text</close>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // mismatching tags
}


TEST_F(DvisvgmSpecialTest, rawdef) {
	std::istringstream iss("rawdef first");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("<defs>first</defs>")) << recorder.defsString();
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));

	iss.clear(); iss.str("rawdef \t <second></second> \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("<defs>first<second/></defs>"));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
}


TEST_F(DvisvgmSpecialTest, pattern1) {
	const auto cmds = {
		"rawset pat1",
		"raw text1",
		"raw <elem>text2</elem>",
		"endrawset",
		"raw first",
		"rawput pat1",
		"rawput pat1",
	};
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.preprocess("", iss, recorder);
	}
	handler.finishPreprocessing();
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.process("", iss, recorder);
	}
	handler.finishPage();
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'>firsttext1<elem>text2</elem>text1<elem>text2</elem></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, pattern2) {
	const auto cmds = {
		"rawset pat2",
		"rawdef text1",
		"rawdef <elem>text2</elem>",
		"endrawset",
		"rawdef first",
		"rawput pat2",
		"rawput pat2",
	};
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.preprocess("", iss, recorder);
	}
	handler.finishPreprocessing();
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.process("", iss, recorder);
	}
	handler.finishPage();
	EXPECT_TRUE(recorder.defsEquals("<defs>firsttext1<elem>text2</elem></defs>")) << recorder.defsString();
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
}


TEST_F(DvisvgmSpecialTest, pattern3) {
	const auto cmds = {
		"rawset pat3",
		"raw <elem first='a' second='x\"y\"'>text<empty/></elem>",
		"rawdef <a/>text2",
		"endrawset",
		"rawdef first",
		"raw second",
		"rawput pat3",
		"rawput pat3",
	};
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.preprocess("", iss, recorder);
	}
	handler.finishPreprocessing();
	for (const char *cmd : cmds) {
		std::istringstream iss(cmd);
		handler.process("", iss, recorder);
	}
	EXPECT_TRUE(recorder.defsEquals("<defs>first<a/>text2</defs>"));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'>second<elem first='a' second='x\"y\"'>text<empty/></elem><elem first='a' second='x\"y\"'>text<empty/></elem></g>")) << recorder.pageString();
	handler.finishPage();
}


TEST_F(DvisvgmSpecialTest, fail1) {
	std::istringstream iss("rawset");  // pattern name missing
	EXPECT_THROW(handler.preprocess("", iss, recorder), SpecialException);
	handler.finishPreprocessing();
}


TEST_F(DvisvgmSpecialTest, fail2) {
	std::istringstream iss("rawset pat");  // endrawset missing
	handler.preprocess("", iss, recorder);
	EXPECT_THROW(handler.finishPreprocessing(), SpecialException);
}


TEST_F(DvisvgmSpecialTest, processImg) {
	std::istringstream iss("img 72.27 72.27 test.png");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><image x='-42' y='14' width='72' height='72' xlink:href='test.png'/></g>")) << recorder.pageString();

	recorder.clear();
	iss.clear();
	iss.str("img 10bp 20bp test2.png");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'><image x='-42' y='14' width='10' height='20' xlink:href='test2.png'/></g>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, fail3) {
	std::istringstream iss("img 10 20xy test.png");  // unknown unit
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);
}


TEST_F(DvisvgmSpecialTest, processBBox) {
	std::istringstream iss("bbox abs 0 0 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<g id='page1'/>"));
	EXPECT_TRUE(recorder.bboxEquals("0 0 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("-42 -58 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox 72bp 72bp");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("-42 -58 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox rel 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("-42 -58 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox new name");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("0 0 0 0"));
}


TEST_F(DvisvgmSpecialTest, fail4) {
	std::istringstream iss("bbox abs 0 0 72.27xx 72.27");  // unknown unit
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);
}
