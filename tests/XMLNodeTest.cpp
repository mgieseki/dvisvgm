/*************************************************************************
** XMLNodeTest.cpp                                                      **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <algorithm>
#include <cstring>
#include "XMLNode.hpp"

using namespace std;


TEST(XMLNodeTest, appendElement) {
	XMLElementNode root("root");
	root.append(new XMLElementNode("child1"));
	root.append(new XMLElementNode("child2"));
	EXPECT_EQ(root.children().size(), 2);
	EXPECT_FALSE(root.empty());
	XMLElementNode *child1 = dynamic_cast<XMLElementNode*>(root.children().front().get());
	XMLElementNode *child2 = dynamic_cast<XMLElementNode*>(root.children().back().get());
	EXPECT_TRUE(child1 != 0);
	EXPECT_TRUE(child2 != 0);
	EXPECT_EQ(string(child1->getName()), "child1");
	EXPECT_EQ(string(child2->getName()), "child2");
	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependElement) {
	XMLElementNode root("root");
	root.prepend(new XMLElementNode("child1"));
	root.prepend(new XMLElementNode("child2"));
	EXPECT_EQ(root.children().size(), 2);
	XMLElementNode *child1 = dynamic_cast<XMLElementNode*>(root.children().front().get());
	XMLElementNode *child2 = dynamic_cast<XMLElementNode*>(root.children().back().get());
	EXPECT_TRUE(child1 != 0);
	EXPECT_TRUE(child2 != 0);
	EXPECT_EQ(string(child1->getName()), "child2");
	EXPECT_EQ(string(child2->getName()), "child1");
}


TEST(XMLNodeTest, appendText) {
	XMLElementNode root("root");
	root.append(new XMLTextNode("first string"));
	EXPECT_EQ(root.children().size(), 1);
	XMLTextNode *lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	EXPECT_TRUE(lastChild != 0);
	EXPECT_EQ(lastChild->getText(), "first string");

	root.append(new XMLTextNode(",second string"));
	EXPECT_EQ(root.children().size(), 1);
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	EXPECT_TRUE(lastChild != 0);
	EXPECT_EQ(lastChild->getText(), "first string,second string");

	root.append(",third string");
	EXPECT_EQ(root.children().size(), 1);
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	EXPECT_TRUE(lastChild != 0);
	EXPECT_EQ(lastChild->getText(), "first string,second string,third string");

	root.append(new XMLElementNode("separator"));
	root.append(",fourth string");
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	EXPECT_TRUE(lastChild != 0);
	EXPECT_EQ(lastChild->getText(), ",fourth string");

	root.append(new XMLElementNode("separator"));
	root.append(new XMLTextNode(",fifth string"));
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	EXPECT_TRUE(lastChild != 0);
	EXPECT_EQ(lastChild->getText(), ",fifth string");

	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependText) {
	XMLElementNode root("root");
	root.prepend(new XMLTextNode("first string"));
	EXPECT_EQ(root.children().size(), 1);
	XMLTextNode *firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	EXPECT_TRUE(firstChild != 0);
	EXPECT_EQ(firstChild->getText(), "first string");

	root.prepend(new XMLTextNode("second string,"));
	EXPECT_EQ(root.children().size(), 1);
	firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	EXPECT_TRUE(firstChild != 0);
	EXPECT_EQ(firstChild->getText(), "second string,first string");

	root.prepend(new XMLElementNode("separator"));
	root.prepend(new XMLTextNode("third string,"));
	firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	EXPECT_TRUE(firstChild != 0);
	EXPECT_EQ(firstChild->getText(), "third string,");
}


TEST(XMLNodeTest, attributes) {
	XMLElementNode root("root");
	root.addAttribute("string", "text");
	root.addAttribute("integer", 42);
	root.addAttribute("double", 42.24);
	EXPECT_TRUE(root.empty());
	EXPECT_TRUE(root.hasAttribute("string"));
	EXPECT_TRUE(root.hasAttribute("integer"));
	EXPECT_TRUE(root.hasAttribute("double"));
	EXPECT_FALSE(root.hasAttribute("noname	"));
	EXPECT_EQ(string(root.getAttributeValue("string")), "text");
	EXPECT_EQ(string(root.getAttributeValue("integer")), "42");
	EXPECT_EQ(string(root.getAttributeValue("double")), "42.24");
	EXPECT_TRUE(root.getAttributeValue("none") == 0);
}


TEST(XMLNodeTest, clone) {
	XMLElementNode root ("root");
	root.addAttribute("string", "text");
	root.addAttribute("integer", 42);
	root.addAttribute("double", 42.24);
	root.append("text");
	XMLElementNode *clone = root.clone();
	EXPECT_EQ(clone->children().size(), 1);
	EXPECT_EQ(string(clone->getAttributeValue("string")), "text");
	EXPECT_EQ(string(clone->getAttributeValue("integer")), "42");
	EXPECT_EQ(string(clone->getAttributeValue("double")), "42.24");
	delete clone;
}


TEST(XMLNodeTest, insertBefore) {
	XMLElementNode root("root");
	XMLElementNode *child1 = new XMLElementNode("child1");
	XMLElementNode *child2 = new XMLElementNode("child2");
	root.append(child1);
	root.append(child2);
	XMLElementNode *node = new XMLElementNode("node");
	EXPECT_FALSE(root.insertBefore(child1, node));
	delete node;
	EXPECT_EQ(root.children().size(), 2);
	EXPECT_TRUE(root.insertBefore(new XMLElementNode("child3"), child1));
	EXPECT_EQ(root.children().size(), 3);
	XMLElementNode *child = dynamic_cast<XMLElementNode*>(root.children().front().get());
	EXPECT_EQ(string(child->getName()), "child3");
	EXPECT_TRUE(root.insertBefore(new XMLElementNode("child4"), child2));
	const char *names[] = {"child3", "child1", "child4", "child2"};
	const char **p = names;
	for (const auto &node : root.children()) {
		XMLElementNode *elem = dynamic_cast<XMLElementNode*>(node.get());
		ASSERT_NE(elem, nullptr);
		EXPECT_EQ(elem->getName(), *p++) << "name=" << elem->getName();
	}
}


TEST(XMLNodeTest, insertAfter) {
	XMLElementNode root("root");
	XMLElementNode *child1 = new XMLElementNode("child1");
	XMLElementNode *child2 = new XMLElementNode("child2");
	root.append(child1);
	root.append(child2);
	XMLElementNode *node = new XMLElementNode("node");
	EXPECT_FALSE(root.insertAfter(child1, node));
	delete node;
	EXPECT_EQ(root.children().size(), 2);
	EXPECT_TRUE(root.insertAfter(new XMLElementNode("child3"), child1));
	EXPECT_TRUE(root.insertAfter(new XMLElementNode("child4"), child2));
	EXPECT_EQ(root.children().size(), 4);
	const char *names[] = {"child1", "child3", "child2", "child4"};
	const char **p = names;
	for (const auto &node : root.children()) {
		XMLElementNode *elem = dynamic_cast<XMLElementNode*>(node.get());
		ASSERT_NE(elem, nullptr);
		EXPECT_EQ(elem->getName(), *p++) << "name=" << elem->getName();
	}
}


TEST(XMLNodeTest, getDescendants) {
	XMLElementNode root("root");
	XMLElementNode *child1 = new XMLElementNode("child");
	XMLElementNode *child2 = new XMLElementNode("childX");
	XMLElementNode *child3 = new XMLElementNode("child");
	XMLElementNode *child4 = new XMLElementNode("child");
	root.append(child1);
	child1->addAttribute("attr", "value");
	child1->append(child2);
	child2->addAttribute("attr", "value");
	child2->append(child3);
	child3->addAttribute("attr", "value");
	child3->append("text");
	root.append(child4);
	vector<XMLElementNode*> elements;
	root.getDescendants("child", 0, elements);
	EXPECT_EQ(elements.size(), 3);
	{
		XMLElementNode *nodes[] = {child1, child3, child4};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants("child", "attr", elements);
		EXPECT_EQ(elements.size(), 2);
		XMLElementNode *nodes[] = {child1, child3};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants(0, "attr", elements);
		EXPECT_EQ(elements.size(), 3);
		XMLElementNode *nodes[] = {child1, child2, child3};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}
}


TEST(XMLNodeTest, getFirstDescendant) {
	XMLElementNode root("root");
	XMLElementNode *child1 = new XMLElementNode("child");
	XMLElementNode *child2 = new XMLElementNode("childX");
	XMLElementNode *child3 = new XMLElementNode("child");
	XMLElementNode *child4 = new XMLElementNode("child");
	root.append(child1);
	child1->addAttribute("attr", "valueX");
	child1->append(child2);
	child2->addAttribute("attr", "value");
	child2->append(child3);
	child3->addAttribute("attrX", "value");
	child3->append("text");
	root.append(child4);
	EXPECT_EQ(root.getFirstDescendant("child", 0, 0), child1);
	EXPECT_EQ(root.getFirstDescendant("child", "attr", 0), child1);
	EXPECT_EQ(root.getFirstDescendant("child", "attrX", 0), child3);
	EXPECT_EQ(root.getFirstDescendant("child", "attrX", "value"), child3);
	EXPECT_EQ(root.getFirstDescendant(0, "attrX", "value"), child3);
	EXPECT_EQ(root.getFirstDescendant(0, "attrX", 0), child3);
	EXPECT_TRUE(root.getFirstDescendant("child", "attr", "value") == 0);
}


TEST(XMLNodeTest, write) {
	XMLElementNode root("root");
	XMLElementNode *child1 = new XMLElementNode("child");
	XMLElementNode *child2 = new XMLElementNode("childX");
	XMLElementNode *child3 = new XMLElementNode("child");
	XMLElementNode *child4 = new XMLElementNode("child");
	root.append(child1);
	child1->addAttribute("attr", "valueX");
	child1->append(child2);
	child2->addAttribute("attr", "value");
	child2->append(child3);
	child3->addAttribute("attrX", "value");
	child3->append("text");
	root.append(child4);
	ostringstream oss;
	root.write(oss);
	string str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><child attr='valueX'><childX attr='value'><child attrX='value'>text</child></childX></child><child/></root>");
}


TEST(XMLNodeTest, cdata) {
	XMLElementNode root("root");
	XMLCDataNode *cdataNode = new XMLCDataNode("text & <text>");
	root.append(cdataNode);
	root.append(new XMLElementNode("element"));
	root.append(cdataNode->clone());
	ostringstream oss;
	root.write(oss);
	string str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><![CDATA[text & <text>]]><element/><![CDATA[text & <text>]]></root>");

	cdataNode->clear();
	oss.str("");
	root.write(oss);
	str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><element/><![CDATA[text & <text>]]></root>");
}

