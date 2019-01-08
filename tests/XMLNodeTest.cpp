/*************************************************************************
** XMLNodeTest.cpp                                                      **
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
#include <algorithm>
#include <cstring>
#include <memory>
#include "utility.hpp"
#include "XMLNode.hpp"

using namespace std;


TEST(XMLNodeTest, appendElement) {
	XMLElementNode root("root");
	root.append(util::make_unique<XMLElementNode>("child1"));
	root.append(util::make_unique<XMLElementNode>("child2"));
	EXPECT_EQ(root.children().size(), 2u);
	EXPECT_FALSE(root.empty());
	XMLElementNode *child1 = dynamic_cast<XMLElementNode*>(root.children().front().get());
	XMLElementNode *child2 = dynamic_cast<XMLElementNode*>(root.children().back().get());
	ASSERT_NE(child1, nullptr);
	ASSERT_NE(child2, nullptr);
	EXPECT_EQ(child1->getName(), "child1");
	EXPECT_EQ(child2->getName(), "child2");
	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependElement) {
	XMLElementNode root("root");
	root.prepend(util::make_unique<XMLElementNode>("child1"));
	root.prepend(util::make_unique<XMLElementNode>("child2"));
	EXPECT_EQ(root.children().size(), 2u);
	XMLElementNode *child1 = dynamic_cast<XMLElementNode*>(root.children().front().get());
	XMLElementNode *child2 = dynamic_cast<XMLElementNode*>(root.children().back().get());
	ASSERT_NE(child1, nullptr);
	ASSERT_NE(child2, nullptr);
	EXPECT_EQ(child1->getName(), "child2");
	EXPECT_EQ(child2->getName(), "child1");
}


TEST(XMLNodeTest, appendText) {
	XMLElementNode root("root");
	root.append(util::make_unique<XMLTextNode>("first string"));
	EXPECT_EQ(root.children().size(), 1u);
	XMLTextNode *lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string");

	root.append(util::make_unique<XMLTextNode>(",second string"));
	EXPECT_EQ(root.children().size(), 1u);
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string,second string");

	root.append(",third string");
	EXPECT_EQ(root.children().size(), 1u);
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string,second string,third string");

	root.append(util::make_unique<XMLElementNode>("separator"));
	root.append(",fourth string");
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), ",fourth string");

	root.append(util::make_unique<XMLElementNode>("separator"));
	root.append(util::make_unique<XMLTextNode>(",fifth string"));
	lastChild = dynamic_cast<XMLTextNode*>(root.children().back().get());
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), ",fifth string");

	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependText) {
	XMLElementNode root("root");
	root.prepend(util::make_unique<XMLTextNode>("first string"));
	EXPECT_EQ(root.children().size(), 1u);
	XMLTextNode *firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	ASSERT_NE(firstChild, nullptr);
	EXPECT_EQ(firstChild->getText(), "first string");

	root.prepend(util::make_unique<XMLTextNode>("second string,"));
	EXPECT_EQ(root.children().size(), 1u);
	firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	ASSERT_NE(firstChild, nullptr);
	EXPECT_EQ(firstChild->getText(), "second string,first string");

	root.prepend(util::make_unique<XMLElementNode>("separator"));
	root.prepend(util::make_unique<XMLTextNode>("third string,"));
	firstChild = dynamic_cast<XMLTextNode*>(root.children().front().get());
	ASSERT_NE(firstChild, nullptr);
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
	EXPECT_STREQ(root.getAttributeValue("string"), "text");
	EXPECT_STREQ(root.getAttributeValue("integer"), "42");
	EXPECT_STREQ(root.getAttributeValue("double"), "42.24");
	EXPECT_EQ(root.getAttributeValue("none"), nullptr);
}


TEST(XMLNodeTest, clone) {
	XMLElementNode root ("root");
	root.addAttribute("string", "text");
	root.addAttribute("integer", 42);
	root.addAttribute("double", 42.24);
	root.append("text");
	unique_ptr<XMLElementNode> clone = util::static_unique_ptr_cast<XMLElementNode>(root.clone());
	EXPECT_EQ(clone->children().size(), 1u);
	EXPECT_STREQ(clone->getAttributeValue("string"), "text");
	EXPECT_STREQ(clone->getAttributeValue("integer"), "42");
	EXPECT_STREQ(clone->getAttributeValue("double"), "42.24");
}


TEST(XMLNodeTest, insertBefore) {
	XMLElementNode root("root");
	auto child1 = util::make_unique<XMLElementNode>("child1");
	auto child2 = util::make_unique<XMLElementNode>("child2");
	XMLNode* child1Ptr = root.append(std::move(child1));
	XMLNode* child2Ptr = root.append(std::move(child2));
	auto node = util::make_unique<XMLElementNode>("node");
	EXPECT_FALSE(root.insertBefore(util::make_unique<XMLElementNode>("dummy"), node.get()));
	EXPECT_EQ(root.children().size(), 2u);
	EXPECT_TRUE(root.insertBefore(util::make_unique<XMLElementNode>("child3"), child1Ptr));
	EXPECT_EQ(root.children().size(), 3u);
	XMLElementNode *child = dynamic_cast<XMLElementNode*>(root.children().front().get());
	EXPECT_EQ(child->getName(), "child3");
	EXPECT_TRUE(root.insertBefore(util::make_unique<XMLElementNode>("child4"), child2Ptr));
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
	auto child1 = util::make_unique<XMLElementNode>("child1");
	auto child2 = util::make_unique<XMLElementNode>("child2");
	XMLNode *child1Ptr = root.append(std::move(child1));
	XMLNode *child2Ptr =root.append(std::move(child2));
	auto node = util::make_unique<XMLElementNode>("node");
	EXPECT_FALSE(root.insertAfter(util::make_unique<XMLElementNode>("dummy"), node.get()));
	EXPECT_EQ(root.children().size(), 2u);
	EXPECT_TRUE(root.insertAfter(util::make_unique<XMLElementNode>("child3"), child1Ptr));
	EXPECT_TRUE(root.insertAfter(util::make_unique<XMLElementNode>("child4"), child2Ptr));
	EXPECT_EQ(root.children().size(), 4u);
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
	auto child1 = util::make_unique<XMLElementNode>("child");
	auto child2 = util::make_unique<XMLElementNode>("childX");
	auto child3 = util::make_unique<XMLElementNode>("child");
	auto child4 = util::make_unique<XMLElementNode>("child");
	child1->addAttribute("attr", "value");
	child2->addAttribute("attr", "value");
	child3->addAttribute("attr", "value");
	child3->append("text");
	XMLElementNode *child3Ptr = static_cast<XMLElementNode*>(child2->append(std::move(child3)));
	XMLElementNode *child2Ptr = static_cast<XMLElementNode*>(child1->append(std::move(child2)));
	XMLElementNode *child1Ptr = static_cast<XMLElementNode*>(root.append(std::move(child1)));
	XMLElementNode *child4Ptr = static_cast<XMLElementNode*>(root.append(std::move(child4)));
	vector<XMLElementNode*> elements;
	root.getDescendants("child", nullptr, elements);
	EXPECT_EQ(elements.size(), 3u);
	{
		XMLElementNode *nodes[] = {child1Ptr, child3Ptr, child4Ptr};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants("child", "attr", elements);
		EXPECT_EQ(elements.size(), 2u);
		XMLElementNode *nodes[] = {child1Ptr, child3Ptr};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants(nullptr, "attr", elements);
		EXPECT_EQ(elements.size(), 3u);
		XMLElementNode *nodes[] = {child1Ptr, child2Ptr, child3Ptr};
		XMLElementNode **p = nodes;
		for (const XMLElementNode *elem : elements)
			EXPECT_EQ(elem, *p++);
	}
}


TEST(XMLNodeTest, getFirstDescendant) {
	XMLElementNode root("root");
	auto child1 = util::make_unique<XMLElementNode>("child");
	auto child2 = util::make_unique<XMLElementNode>("childX");
	auto child3 = util::make_unique<XMLElementNode>("child");
	auto child4 = util::make_unique<XMLElementNode>("child");
	child1->addAttribute("attr", "valueX");
	child2->addAttribute("attr", "value");
	child3->addAttribute("attrX", "value");
	child3->append("text");
	XMLNode *child3Ptr = child2->append(std::move(child3));
	child1->append(std::move(child2));
	XMLNode *child1Ptr = root.append(std::move(child1));
	root.append(std::move(child4));
	EXPECT_EQ(root.getFirstDescendant("child", nullptr, nullptr), child1Ptr);
	EXPECT_EQ(root.getFirstDescendant("child", "attr", nullptr), child1Ptr);
	EXPECT_EQ(root.getFirstDescendant("child", "attrX", nullptr), child3Ptr);
	EXPECT_EQ(root.getFirstDescendant("child", "attrX", "value"), child3Ptr);
	EXPECT_EQ(root.getFirstDescendant(0, "attrX", "value"), child3Ptr);
	EXPECT_EQ(root.getFirstDescendant(0, "attrX", nullptr), child3Ptr);
	EXPECT_EQ(root.getFirstDescendant("child", "attr", "value"), nullptr);
}


TEST(XMLNodeTest, write) {
	XMLElementNode root("root");
	auto child1 = util::make_unique<XMLElementNode>("child");
	auto child2 = util::make_unique<XMLElementNode>("childX");
	auto child3 = util::make_unique<XMLElementNode>("child");
	auto child4 = util::make_unique<XMLElementNode>("child");
	child1->addAttribute("attr", "valueX");
	child2->addAttribute("attr", "value");
	child3->addAttribute("attrX", "value");
	child3->append("text");
	child2->append(std::move(child3));
	child1->append(std::move(child2));
	root.append(std::move(child1));
	root.append(std::move(child4));
	ostringstream oss;
	root.write(oss);
	string str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><child attr='valueX'><childX attr='value'><child attrX='value'>text</child></childX></child><child/></root>");
}


TEST(XMLNodeTest, cdata) {
	XMLElementNode root("root");
	auto cdataNode = util::make_unique<XMLCDataNode>("text & <text>");
	XMLNode *cdataNodePtr = root.append(std::move(cdataNode));
	root.append(util::make_unique<XMLElementNode>("element"));
	root.append(cdataNodePtr->clone());
	ostringstream oss;
	root.write(oss);
	string str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><![CDATA[text & <text>]]><element/><![CDATA[text & <text>]]></root>");

	cdataNodePtr->clear();
	oss.str("");
	root.write(oss);
	str = oss.str();
	str.erase(remove(str.begin(), str.end(), '\n'), str.end());
	EXPECT_EQ(str, "<root><element/><![CDATA[text & <text>]]></root>");
}

