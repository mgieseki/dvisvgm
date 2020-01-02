/*************************************************************************
** PapersizeSpecialTest.cpp                                             **
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
#include "PapersizeSpecialHandler.hpp"
#include "SpecialActions.hpp"

using namespace std;

class PapersizeSpecialTest : public ::testing::Test {
	protected:
		class PapersizeSpecialActions : public EmptySpecialActions {
			public:
				PapersizeSpecialActions () : _pageno(0) {}
				BoundingBox& bbox () override {return _bbox;}
				string getBBoxFormatString () const override    {return _bboxFormatString;}
				void setBBoxFormatString (const string &str)    {_bboxFormatString = str;}
				unsigned getCurrentPageNumber () const override {return _pageno;}
				void setCurrentPageNumber (unsigned pageno)     {_pageno = pageno;}

			private:
				BoundingBox _bbox;
				string _bboxFormatString;
				unsigned _pageno;
		};

		class MyPapersizeSpecialHandler : public PapersizeSpecialHandler {
			public:
				void endPage (PapersizeSpecialActions &actions) {dviEndPage(actions.getCurrentPageNumber(), actions);}
		};


		void SetUp () override {
			handler.reset();
		}

		string bboxString () {
			return actions.bbox().toSVGViewBox();
		}

		void preprocess (const string &str) {
			istringstream iss(str);
			handler.preprocess("", iss, actions);
		}

		bool process (const string &str) {
			istringstream iss(str);
			return handler.process("", iss, actions);
		}

		void endPage () {
			handler.endPage(actions);
		}

	protected:
		MyPapersizeSpecialHandler handler;
		PapersizeSpecialActions actions;
};


TEST_F(PapersizeSpecialTest, name) {
	EXPECT_EQ(handler.name(), "papersize");
	ASSERT_EQ(handler.prefixes().size(), 1u);
	ASSERT_STREQ(handler.prefixes()[0], "papersize=");
}


TEST_F(PapersizeSpecialTest, disabled) {
	actions.setBBoxFormatString("min");  // --bbox=min => ignore papersize specials
	actions.setCurrentPageNumber(1);
	ASSERT_EQ(bboxString(), "0 0 0 0");
	preprocess("123,456");
	endPage();
	ASSERT_EQ(bboxString(), "0 0 0 0");
	ASSERT_TRUE(process("dummy"));
}


TEST_F(PapersizeSpecialTest, singlePage1) {
	actions.setBBoxFormatString("papersize");  // --bbox=papersize => evaluate papersize specials
	actions.setCurrentPageNumber(1);
	ASSERT_EQ(bboxString(), "0 0 0 0");
	preprocess("123bp,456bp");
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 123 456");
}


TEST_F(PapersizeSpecialTest, singlePage2) {
	actions.setBBoxFormatString("papersize");
	actions.setCurrentPageNumber(1);
	ASSERT_EQ(bboxString(), "0 0 0 0");
	preprocess("123bp,456bp");
	preprocess("333bp,444bp");
	preprocess("321bp,654bp");
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 321 654");
}


TEST_F(PapersizeSpecialTest, multiPage1) {
	actions.setBBoxFormatString("papersize");
	actions.setCurrentPageNumber(1);
	ASSERT_EQ(bboxString(), "0 0 0 0");
	preprocess("123bp,456bp");
	preprocess("333bp,444bp");
	preprocess("321bp,654bp");
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 321 654");
	actions.setCurrentPageNumber(2);
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 321 654");
	actions.setCurrentPageNumber(3);
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 321 654");
}


TEST_F(PapersizeSpecialTest, multiPage2) {
	actions.setBBoxFormatString("papersize");
	actions.setCurrentPageNumber(1);
	ASSERT_EQ(bboxString(), "0 0 0 0");
	preprocess("123bp,456bp");
	preprocess("333bp");
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 333 333");
	actions.setCurrentPageNumber(2);
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 333 333");
	actions.setCurrentPageNumber(3);
	preprocess("321bp,456bp");
	endPage();
	ASSERT_EQ(bboxString(), "-72 -72 321 456");
}
