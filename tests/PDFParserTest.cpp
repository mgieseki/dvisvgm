/*************************************************************************
** PDFParserTest.cpp                                                    **
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
#include "PDFParser.hpp"

using namespace std;

TEST(PDFParserTest, empty) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("");
	EXPECT_TRUE(objects.empty());
	objects = parser.parse("  \n   \r  %comment 1 2 3 ");
	EXPECT_TRUE(objects.empty());
}


TEST(PDFParserTest, numbers) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("5 1 +2 -3 1.1 9. .1 -7.2");
	ASSERT_EQ(objects.size(), 8u);

	size_t count=0;
	for (int n : {5, 1, 2, -3}) {
		ASSERT_NE(objects[count].get<int>(), nullptr);
		EXPECT_EQ(*objects[count++].get<int>(), n);
	}
	for (double n : {1.1, 9.0, 0.1, -7.2}) {
		ASSERT_NE(objects[count].get<double>(), nullptr);
		EXPECT_DOUBLE_EQ(*objects[count++].get<double>(), n);
	}
}


TEST(PDFParserTest, literal_strings) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("()");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<string>(), nullptr);
	EXPECT_TRUE(objects[0].get<string>()->empty());

	objects = parser.parse("(literal string) (balanced (bra(ck)ets) inside)(a\\n\\(b\\c)");
	ASSERT_EQ(objects.size(), 3u);

	size_t count=0;
	for (string str : {"literal string", "balanced (bra(ck)ets) inside", "a\n(bc"}) {
		ASSERT_NE(objects[count].get<string>(), nullptr);
		ASSERT_EQ(*objects[count++].get<string>(), str);
	}

	objects = parser.parse(R"*( ( octal \1\12\123\1234 ))*");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<string>(), nullptr);
	EXPECT_EQ(objects[0].get<string>()->size(), 13u);
	EXPECT_EQ(*objects[0].get<string>(), " octal \001\012\123\1234 ");

	objects = parser.parse("(split \\\nline)");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<string>(), nullptr);
	EXPECT_EQ(*objects[0].get<string>(), "split line");

	EXPECT_THROW(parser.parse("(missing parentheses"), PDFException);
}


TEST(PDFParserTest, hex_strings) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("<>< ><\n\r\f>");
	ASSERT_EQ(objects.size(), 3u);
	for (size_t i=0; i < objects.size(); i++) {
		ASSERT_NE(objects[i].get<string>(), nullptr);
		EXPECT_TRUE(objects[i].get<string>()->empty());
	}

	objects = parser.parse("<202>");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<string>(), nullptr);
	EXPECT_EQ(*objects[0].get<string>(), "  ");

	objects = parser.parse("<616263646566 6768\n696A6b6c6D6E6F 7>");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<string>(), nullptr);
	EXPECT_EQ(*objects[0].get<string>(), "abcdefghijklmnop");

	EXPECT_THROW(parser.parse("<abcdefinvalid>"), PDFException);
	EXPECT_THROW(parser.parse("<missing parentheses"), PDFException);
}


TEST(PDFParserTest, arrays) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("[1 2.0/name(string) [5] <6162>]");
	ASSERT_EQ(objects.size(), 1u);
	const PDFArray *arr = objects[0].get<PDFArray>();
	ASSERT_NE(arr, nullptr);
	ASSERT_EQ(arr->size(), 6u);

	// integer number
	ASSERT_NE((*arr)[0].get<int>(), nullptr);
	ASSERT_EQ(*(*arr)[0].get<int>(), 1);

	// real number
	ASSERT_NE((*arr)[1].get<double>(), nullptr);
	ASSERT_DOUBLE_EQ(*(*arr)[1].get<double>(), 2.0);

	// name
	ASSERT_NE((*arr)[2].get<PDFName>(), nullptr);
	ASSERT_EQ(*(*arr)[2].get<PDFName>(), PDFName("name"));

	// literal string
	ASSERT_NE((*arr)[3].get<string>(), nullptr);
	ASSERT_EQ(*(*arr)[3].get<string>(), "string");

	// inner array
	ASSERT_NE((*arr)[4].get<PDFArray>(), nullptr);
	ASSERT_EQ((*arr)[4].get<PDFArray>()->size(), 1u);
	ASSERT_NE((*arr)[4].get<PDFArray>()->at(0).get<int>(), nullptr);
	ASSERT_EQ(*(*arr)[4].get<PDFArray>()->at(0).get<int>(), 5);

	// hex string
	ASSERT_NE((*arr)[5].get<string>(), nullptr);
	ASSERT_EQ(*(*arr)[5].get<string>(), "ab");
}


TEST(PDFParserTest, dictionary) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("<< >>");
	ASSERT_EQ(objects.size(), 1u);
	const PDFDict *dict = objects[0].get<PDFDict>();
	ASSERT_NE(dict, nullptr);
	ASSERT_TRUE(dict->empty());

	objects = parser.parse("<</int 4 /real 5.5/str(string value)/color[0 1 1]>>");
	ASSERT_EQ(objects.size(), 1u);
	dict = objects[0].get<PDFDict>();
	ASSERT_NE(dict, nullptr);
	ASSERT_EQ(dict->size(), 4u);

	// integer number
	ASSERT_NE(dict->find("int"), dict->end());
	ASSERT_NE(dict->find("int")->second.get<int>(), nullptr);
	EXPECT_EQ(*dict->find("int")->second.get<int>(), 4);

	ASSERT_NE(dict->find("real"), dict->end());
	ASSERT_NE(dict->find("real")->second.get<double>(), nullptr);
	EXPECT_DOUBLE_EQ(*dict->find("real")->second.get<double>(), 5.5);

	ASSERT_NE(dict->find("str"), dict->end());
	ASSERT_NE(dict->find("str")->second.get<string>(), nullptr);
	EXPECT_EQ(*dict->find("str")->second.get<string>(), "string value");

	ASSERT_NE(dict->find("color"), dict->end());
	ASSERT_NE(dict->find("color")->second.get<PDFArray>(), nullptr);
	const PDFArray &arr = *dict->find("color")->second.get<PDFArray>();
	ASSERT_EQ(arr.size(), 3u);
	EXPECT_EQ(*arr[0].get<int>(), 0);
	EXPECT_EQ(*arr[1].get<int>(), 1);
	EXPECT_EQ(*arr[2].get<int>(), 1);

	EXPECT_EQ(dict->find("nokey"), dict->end());

	EXPECT_THROW(parser.parse("<</int >>"), PDFException);  // missing value
	EXPECT_THROW(parser.parse("<</int 4>"), PDFException);  // missing ">"
	EXPECT_THROW(parser.parse("<</int 4"), PDFException);   // missing ">>"
}


TEST(PDFParserTest, indirect_objects) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("1 0 obj\n1 2 3 4 5endobj 2 5 R");
	ASSERT_EQ(objects.size(), 2u);
	ASSERT_NE(objects[0].get<PDFIndirectObject>(), nullptr);
	EXPECT_EQ(objects[0].get<PDFIndirectObject>()->objnum, 1);
	EXPECT_EQ(objects[0].get<PDFIndirectObject>()->gennum, 0);

	ASSERT_NE(objects[1].get<PDFObjectRef>(), nullptr);
	EXPECT_EQ(objects[1].get<PDFObjectRef>()->objnum, 2);
	EXPECT_EQ(objects[1].get<PDFObjectRef>()->gennum, 5);

	EXPECT_THROW(parser.parse("1 obj\n1 2 3 4 5endobj"), PDFException);
	EXPECT_THROW(parser.parse("1 (string)obj\n1 2 3 4 5endobj"), PDFException);

	EXPECT_THROW(parser.parse("1 R"), PDFException);
	EXPECT_THROW(parser.parse("1 (string)R"), PDFException);
}


TEST(PDFParserTest, stream) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("stream\n1 2 3 4 endstream");
	ASSERT_EQ(objects.size(), 1u);
	ASSERT_NE(objects[0].get<PDFStream>(), nullptr);
}


TEST(PDFParserTest, ops1) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("@xpos 1op op2 1..2");
	ASSERT_EQ(objects.size(), 4u);
	size_t count=0;
	for (string str : {"@xpos", "1op", "op2", "1..2"}) {
		ASSERT_NE(objects[count].get<PDFOperator>(), nullptr);
		ASSERT_EQ(objects[count++].get<PDFOperator>()->opname, str);
	}
}


TEST(PDFParserTest, ops2) {
	PDFParser parser;
	vector<PDFObject> objects = parser.parse("@xpos 1op op2 @ypos", [](const string &str, vector<PDFObject> &objects) {
		if (str == "@xpos" || str == "@ypos")
			objects.emplace_back(PDFObject(str == "@xpos" ? 1.23 : 3.21));
		else
			objects.emplace_back(PDFOperator(str));
	});
	ASSERT_EQ(objects.size(), 4u);
	size_t count=0;
	for (string str : {"1.23", "1op", "op2", "3.21"}) {
		ASSERT_EQ(string(objects[count++]), str);
	}
}
