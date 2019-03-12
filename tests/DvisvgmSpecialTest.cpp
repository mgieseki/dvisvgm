/*************************************************************************
** DvisvgmSpecialTest.cpp                                               **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2019 Martin Gieseking <martin.gieseking@uos.de>   **
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
		void finishPage ()          {dviEndPage(0, emptyActions);}

	protected:
		EmptySpecialActions emptyActions;
};


class DvisvgmSpecialTest : public ::testing::Test {
	protected:
		class ActionsRecorder : public EmptySpecialActions {
			public:
				ActionsRecorder () : defs(""), page("page")            {context.push_back(&page);}
				void appendToDefs(unique_ptr<XMLNode> &&node) override {defs.append(std::move(node));}
				void appendToPage(unique_ptr<XMLNode> &&node) override {context.back()->append(std::move(node));}
				void pushContextElement (unique_ptr<XMLElement> &&node) override {
					XMLElement *elem = node.get();
					context.back()->append(std::move(node));
					context.push_back(elem);
				}
				void popContextElement () override            {context.pop_back();}
				void embed (const BoundingBox &bb) override   {bbox.embed(bb);}
				double getX () const override                 {return 0;}
				double getY () const override                 {return 0;}
				void clear ()                                 {defs.clear(); page.clear(); bbox=BoundingBox(0, 0, 0, 0);}
				bool defsEquals (const string &str) const     {return defs.getText() == str;}
				bool pageEquals (const string &str) const     {return pageString() == str;}
				bool bboxEquals (const string &str) const     {return bbox.toSVGViewBox() == str;}
				const Matrix& getMatrix () const override     {static Matrix m(1); return m;}
				string bboxString () const                    {return bbox.toSVGViewBox();}
				string pageString () const                    {return toString(context.back());}
				string defsString () const                    {return toString(&defs);}

				void write (ostream &os) const {
					os << "defs: " << defs.getText() << '\n'
						<< "page: " << pageString() << '\n'
						<< "bbox: " << bbox.toSVGViewBox() << '\n';
				}

			protected:
				string toString (const XMLNode *node) const {
					ostringstream oss;
					node->write(oss);
					return oss.str();
				}

			private:
				XMLText defs;
				XMLElement page;
				vector<XMLElement*> context;
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
	istringstream iss("raw first{?nl}");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page>first\n</page>"));

	iss.clear(); iss.str("raw \t second {?bbox dummy} \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page>first\nsecond 0 0 0 0</page>"));
}


TEST_F(DvisvgmSpecialTest, rawElements1) {
	istringstream iss("raw <elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page><elem attr1='1' attr2='20'>text1<inner>&lt;text2</inner>text3</elem></page>"));
}


TEST_F(DvisvgmSpecialTest, rawElements2) {
	istringstream iss("raw <elem attr1='1' attr2='20'>text1");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw text2<inner>text3</inner>text4");
	handler.process("", iss, recorder);
	iss.clear(); iss.str("raw </elem>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page><elem attr1='1' attr2='20'>text1text2<inner>text3</inner>text4</elem></page>"));
}


TEST_F(DvisvgmSpecialTest, rawCDATA) {
	istringstream iss("raw <outer>text1<![CDATA[1 < 2 <!--test-->]]>text2</outer>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page><outer>text1<![CDATA[1 < 2 <!--test-->]]>text2</outer></page>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawComments) {
	istringstream iss("raw <first/><second><!-- 1 < 2 ->--->text</second>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page><first/><second><!-- 1 < 2 ->--->text</second></page>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawPI) {
	istringstream iss("raw <first/><?pi1 whatever?><second><?pi2 whatever?></second>");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page><first/><?pi1 whatever?><second><?pi2 whatever?></second></page>")) << recorder.pageString();
}


TEST_F(DvisvgmSpecialTest, rawElementsFail) {
	istringstream iss("raw <elem attr1='1' attr2='20'");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // incomplete opening tag
	iss.clear(); iss.str("raw </elem>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // spurious closing tag
	iss.clear(); iss.str("raw <open>text</close>");
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);  // mismatching tags
}


TEST_F(DvisvgmSpecialTest, rawdef) {
	std::istringstream iss("rawdef first");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("first"));
	EXPECT_TRUE(recorder.pageEquals("<page/>"));

	iss.clear(); iss.str("rawdef \t second \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("firstsecond"));
	EXPECT_TRUE(recorder.pageEquals("<page/>"));
}


TEST_F(DvisvgmSpecialTest, pattern1) {
	const auto cmds = {
		"rawset pat1",
		"raw text1",
		"raw text2",
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
	EXPECT_TRUE(recorder.pageEquals("<page>firsttext1text2text1text2</page>"));
}


TEST_F(DvisvgmSpecialTest, pattern2) {
	const auto cmds = {
		"rawset pat2",
		"rawdef text1",
		"rawdef text2",
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
	EXPECT_TRUE(recorder.defsEquals("firsttext1text2"));
	EXPECT_TRUE(recorder.pageEquals("<page/>"));
}


TEST_F(DvisvgmSpecialTest, pattern3) {
	const auto cmds = {
		"rawset pat3",
		"raw <elem first='a' second='x\"y\"'>text<empty/></elem>",
		"rawdef text2",
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
	EXPECT_TRUE(recorder.defsEquals("firsttext2"));
	EXPECT_TRUE(recorder.pageEquals("<page>second<elem first='a' second='x\"y\"'>text<empty/></elem><elem first='a' second='x\"y\"'>text<empty/></elem></page>")) << recorder.pageString();
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
	EXPECT_TRUE(recorder.pageEquals("<page><image x='0' y='0' width='72' height='72' xlink:href='test.png'/></page>"));

	recorder.clear();
	iss.clear();
	iss.str("img 10bp 20bp test2.png");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.pageEquals("<page><image x='0' y='0' width='10' height='20' xlink:href='test2.png'/></page>"));
}


TEST_F(DvisvgmSpecialTest, fail3) {
	std::istringstream iss("img 10 20xy test.png");  // unknown unit
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);
}


TEST_F(DvisvgmSpecialTest, processBBox) {
	std::istringstream iss("bbox abs 0 0 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("<page/>"));
	EXPECT_TRUE(recorder.bboxEquals("0 0 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("0 -72 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox 72bp 72bp");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("0 -72 72 72"));

	recorder.clear();
	iss.clear();
	iss.str("bbox rel 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.bboxEquals("0 -72 72 72"));

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
