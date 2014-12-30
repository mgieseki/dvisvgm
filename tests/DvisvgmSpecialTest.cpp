/*************************************************************************
** DvisvgmSpecialTest.cpp                                               **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include "DvisvgmSpecialHandler.h"
#include "SpecialActions.h"
#include "XMLNode.h"

using namespace std;

class DvisvgmSpecialTest : public ::testing::Test
{
	protected:
		class ActionsRecorder : public EmptySpecialActions
		{
			public:
				ActionsRecorder () : defs(""), page("") {}
				void appendToDefs (XMLNode *node)         {defs.append(node);}
				void appendToPage (XMLNode *node)         {page.append(node);}
				void clear ()                             {defs.clear(); page.clear();}
				bool defsEquals (const string &str) const {return defs.getText() == str;}
				bool pageEquals (const string &str) const {return page.getText() == str;}

				void write (ostream &os) const {
					os << "defs: " << defs.getText() << '\n'
						<< "page: " << page.getText() << '\n';
				}

			private:
				XMLTextNode defs, page;
		};

		void SetUp () {
			recorder.clear();
		}

	protected:
		DvisvgmSpecialHandler handler;
		ActionsRecorder recorder;
};


TEST_F(DvisvgmSpecialTest, raw) {
	istringstream iss("raw first");
	handler.process(0, iss, &recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("first"));

	iss.clear(); iss.str("raw \t second \t");
	handler.process(0, iss, &recorder);
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("firstsecond"));
}


TEST_F(DvisvgmSpecialTest, rawdef) {
	std::istringstream iss("rawdef first");
	handler.process(0, iss, &recorder);
	EXPECT_TRUE(recorder.defsEquals("first"));
	EXPECT_TRUE(recorder.pageEquals(""));

	iss.clear(); iss.str("rawdef \t second \t");
	handler.process(0, iss, &recorder);
	EXPECT_TRUE(recorder.defsEquals("firstsecond"));
	EXPECT_TRUE(recorder.pageEquals(""));
}


TEST_F(DvisvgmSpecialTest, pattern1) {
	const char *cmds[] = {
		"rawset pat1",
		"raw text1",
		"raw text2",
		"endrawset",
		"raw first",
		"rawput pat1",
		"rawput pat1",
	};
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.preprocess(0, iss, &recorder);
	}
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.process(0, iss, &recorder);
	}
	EXPECT_TRUE(recorder.defsEquals(""));
	EXPECT_TRUE(recorder.pageEquals("firsttext1text2text1text2"));
}


TEST_F(DvisvgmSpecialTest, pattern2) {
	const char *cmds[] = {
		"rawset pat2",
		"rawdef text1",
		"rawdef text2",
		"endrawset",
		"rawdef first",
		"rawput pat2",
		"rawput pat2",
	};
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.preprocess(0, iss, &recorder);
	}
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.process(0, iss, &recorder);
	}
	EXPECT_TRUE(recorder.defsEquals("firsttext1text2"));
	EXPECT_TRUE(recorder.pageEquals(""));
}


TEST_F(DvisvgmSpecialTest, pattern3) {
	const char *cmds[] = {
		"rawset pat3",
		"raw text1",
		"rawdef text2",
		"endrawset",
		"rawdef first",
		"raw second",
		"rawput pat3",
		"rawput pat3",
	};
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.preprocess(0, iss, &recorder);
	}
	for (size_t i=0; i < sizeof(cmds)/sizeof(char*); i++) {
		std::istringstream iss(cmds[i]);
		handler.process(0, iss, &recorder);
	}
	EXPECT_TRUE(recorder.defsEquals("firsttext2"));
	EXPECT_TRUE(recorder.pageEquals("secondtext1text1"));
}


TEST_F(DvisvgmSpecialTest, fail1) {
	std::istringstream iss("rawset");
	EXPECT_THROW(handler.preprocess(0, iss, &recorder), SpecialException);
}
