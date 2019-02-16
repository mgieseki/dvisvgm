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
				ActionsRecorder () : defs(""), page("") {}
				void appendToDefs(unique_ptr<XMLNode> &&node) override {defs.append(std::move(node));}
				void appendToPage(unique_ptr<XMLNode> &&node) override {page.append(std::move(node));}
				void embed (const BoundingBox &bb) override            {bbox.embed(bb);}
				double getX () const override                          {return 0;}
				double getY () const override                          {return 0;}
				void clear ()                                          {defs.clear(); page.clear(); bbox=BoundingBox(0, 0, 0, 0);}
				bool defsEquals (const string &str) const              {return defs.getText() == str;}
				bool pageEquals (const string &str) const              {return page.getText() == str;}
				bool bboxEquals (const string &str) const              {return bbox.toSVGViewBox() == str;}
				const Matrix& getMatrix () const override              {static Matrix m(1); return m;}
				string bboxString () const                             {return bbox.toSVGViewBox();}
				string pageString () const                             {return page.getText();}

				void write (ostream &os) const {
					os << "defs: " << defs.getText() << '\n'
						<< "page: " << page.getText() << '\n'
						<< "bbox: " << bbox.toSVGViewBox() << '\n';
				}

			private:
				XMLTextNode defs, page;
				BoundingBox bbox;
		};

		void SetUp () override {
			recorder.clear();
		}

	protected:
		MyDvisvgmSpecialHandler handler;
		ActionsRecorder recorder;
};


TEST_F(DvisvgmSpecialTest, basic) {
	EXPECT_EQ(handler.name(), "dvisvgm");
}


TEST_F(DvisvgmSpecialTest, raw) {
	istringstream iss("raw first{?nl}");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("first\n"));

	iss.clear(); iss.str("raw \t second {?bbox dummy} \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("first\nsecond 0 0 0 0"));
}


TEST_F(DvisvgmSpecialTest, rawdef) {
	std::istringstream iss("rawdef first");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("first"));
	EXPECT_TRUE(recorder.pageEquals(""));

	iss.clear(); iss.str("rawdef \t second \t");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals("firstsecond"));
	EXPECT_TRUE(recorder.pageEquals(""));
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
	EXPECT_TRUE(recorder.pageEquals("firsttext1text2text1text2"));
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
	EXPECT_TRUE(recorder.pageEquals(""));
}


TEST_F(DvisvgmSpecialTest, pattern3) {
	const auto cmds = {
		"rawset pat3",
		"raw text1",
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
	EXPECT_TRUE(recorder.pageEquals("secondtext1text1"));
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
	EXPECT_TRUE(recorder.pageEquals("&lt;image x=&apos;0&apos; y=&apos;0&apos; width=&apos;72&apos; height=&apos;72&apos; xlink:href=&apos;test.png&apos;/>"));

	recorder.clear();
	iss.clear();
	iss.str("img 10bp 20bp test2.png");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.pageEquals("&lt;image x=&apos;0&apos; y=&apos;0&apos; width=&apos;10&apos; height=&apos;20&apos; xlink:href=&apos;test2.png&apos;/>"));
}


TEST_F(DvisvgmSpecialTest, fail3) {
	std::istringstream iss("img 10 20xy test.png");  // unknown unit
	EXPECT_THROW(handler.process("", iss, recorder), SpecialException);
}


TEST_F(DvisvgmSpecialTest, processBBox) {
	std::istringstream iss("bbox abs 0 0 72.27 72.27");
	handler.process("", iss, recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals(""));
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
