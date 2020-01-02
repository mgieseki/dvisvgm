/*************************************************************************
** XMLNodeTest.cpp                                                      **
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
#include <algorithm>
#include <cstring>
#include <memory>
#include "utility.hpp"
#include "XMLNode.hpp"

using namespace std;


TEST(XMLNodeTest, downcast) {
	unique_ptr<XMLNode> elem = util::make_unique<XMLElement>("element");
	EXPECT_EQ(elem->toElement(), elem.get());
	EXPECT_EQ(elem->toText(), nullptr);
	EXPECT_EQ(elem->toCData(), nullptr);
	EXPECT_EQ(elem->toComment(), nullptr);

	unique_ptr<XMLNode> text = util::make_unique<XMLText>("text");
	EXPECT_EQ(text->toElement(), nullptr);
	EXPECT_EQ(text->toText(), text.get());
	EXPECT_EQ(text->toCData(), nullptr);
	EXPECT_EQ(text->toComment(), nullptr);

	unique_ptr<XMLNode> cdata = util::make_unique<XMLCData>("cdata");
	EXPECT_EQ(cdata->toElement(), nullptr);
	EXPECT_EQ(cdata->toText(), nullptr);
	EXPECT_EQ(cdata->toCData(), cdata.get());
	EXPECT_EQ(cdata->toComment(), nullptr);

	unique_ptr<XMLNode> comment = util::make_unique<XMLComment>("comment");
	EXPECT_EQ(comment->toElement(), nullptr);
	EXPECT_EQ(comment->toText(), nullptr);
	EXPECT_EQ(comment->toCData(), nullptr);
	EXPECT_EQ(comment->toComment(), comment.get());
}


static int number_of_children (XMLElement &elem) {
	int count=0;
	for (XMLNode *node = elem.firstChild(); node; node=node->next())
		count++;
	return count;
}


TEST(XMLNodeTest, appendElement) {
	XMLElement root("root");
	root.append(util::make_unique<XMLElement>("child1"));
	root.append(util::make_unique<XMLElement>("child2"));
	EXPECT_EQ(number_of_children(root), 2);
	EXPECT_FALSE(root.empty());
	XMLElement *child1 = root.firstChild()->toElement();
	XMLElement *child2 = root.lastChild()->toElement();
	ASSERT_NE(child1, nullptr);
	EXPECT_EQ(child1->parent(), &root);
	EXPECT_EQ(child1->prev(), nullptr);
	EXPECT_EQ(child1->next(), child2);
	EXPECT_EQ(child1->firstChild(), nullptr);
	EXPECT_EQ(child1->lastChild(), nullptr);
	EXPECT_EQ(child1->name(), "child1");

	ASSERT_NE(child2, nullptr);
	EXPECT_EQ(child2->parent(), &root);
	EXPECT_EQ(child2->prev(), child1);
	EXPECT_EQ(child2->next(), nullptr);
	EXPECT_EQ(child2->firstChild(), nullptr);
	EXPECT_EQ(child2->lastChild(), nullptr);
	EXPECT_EQ(child2->name(), "child2");
	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependElement) {
	XMLElement root("root");
	root.prepend(util::make_unique<XMLElement>("child1"));
	root.prepend(util::make_unique<XMLElement>("child2"));
	EXPECT_EQ(number_of_children(root), 2);
	XMLElement *child1 = root.lastChild()->toElement();
	XMLElement *child2 = root.firstChild()->toElement();
	ASSERT_NE(child1, nullptr);
	EXPECT_EQ(child1->parent(), &root);
	EXPECT_EQ(child1->prev(), child2);
	EXPECT_EQ(child1->next(), nullptr);
	EXPECT_EQ(child1->firstChild(), nullptr);
	EXPECT_EQ(child1->lastChild(), nullptr);
	EXPECT_EQ(child1->name(), "child1");

	ASSERT_NE(child2, nullptr);
	EXPECT_EQ(child2->parent(), &root);
	EXPECT_EQ(child2->prev(), nullptr);
	EXPECT_EQ(child2->next(), child1);
	EXPECT_EQ(child2->firstChild(), nullptr);
	EXPECT_EQ(child2->lastChild(), nullptr);
	EXPECT_EQ(child2->name(), "child2");
}


TEST(XMLNodeTest, appendText) {
	XMLElement root("root");
	root.append(util::make_unique<XMLText>("first string"));
	EXPECT_EQ(number_of_children(root), 1);
	XMLText *lastChild = root.lastChild()->toText();
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string");
	EXPECT_EQ(lastChild->parent(), &root);
	EXPECT_EQ(lastChild->prev(), nullptr);
	EXPECT_EQ(lastChild->next(), nullptr);
	EXPECT_EQ(root.firstChild(), root.lastChild());

	root.append(util::make_unique<XMLText>(",second string"));
	EXPECT_EQ(number_of_children(root), 1);
	lastChild = root.lastChild()->toText();
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string,second string");
	EXPECT_EQ(lastChild->parent(), &root);
	EXPECT_EQ(lastChild->prev(), nullptr);
	EXPECT_EQ(lastChild->next(), nullptr);
	EXPECT_EQ(root.firstChild(), root.lastChild());

	root.append(",third string");
	EXPECT_EQ(number_of_children(root), 1);
	lastChild = root.lastChild()->toText();
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), "first string,second string,third string");
	EXPECT_EQ(lastChild->parent(), &root);
	EXPECT_EQ(lastChild->prev(), nullptr);
	EXPECT_EQ(lastChild->next(), nullptr);

	root.append(util::make_unique<XMLElement>("separator"));
	root.append(",fourth string");
	EXPECT_EQ(number_of_children(root), 3);
	lastChild = root.lastChild()->toText();
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), ",fourth string");

	root.append(util::make_unique<XMLElement>("separator"));
	root.append(util::make_unique<XMLText>(",fifth string"));
	EXPECT_EQ(number_of_children(root), 5);
	lastChild = root.lastChild()->toText();
	ASSERT_NE(lastChild, nullptr);
	EXPECT_EQ(lastChild->getText(), ",fifth string");

	root.clear();
	EXPECT_TRUE(root.empty());
}


TEST(XMLNodeTest, prependText) {
	XMLElement root("root");
	root.prepend(util::make_unique<XMLText>("first string"));
	EXPECT_EQ(number_of_children(root), 1);
	XMLText *firstChild = root.firstChild()->toText();
	ASSERT_NE(firstChild, nullptr);
	EXPECT_EQ(firstChild->getText(), "first string");
	EXPECT_EQ(firstChild->parent(), &root);
	EXPECT_EQ(firstChild->prev(), nullptr);
	EXPECT_EQ(firstChild->next(), nullptr);

	root.prepend(util::make_unique<XMLText>("second string,"));
	EXPECT_EQ(number_of_children(root), 1);
	firstChild = root.firstChild()->toText();
	ASSERT_NE(firstChild, nullptr);
	EXPECT_EQ(firstChild->getText(), "second string,first string");
	EXPECT_EQ(firstChild->parent(), &root);
	EXPECT_EQ(firstChild->prev(), nullptr);
	EXPECT_EQ(firstChild->next(), nullptr);

	root.prepend(util::make_unique<XMLElement>("separator"));
	root.prepend(util::make_unique<XMLText>("third string,"));
	EXPECT_EQ(number_of_children(root), 3);
	firstChild = root.firstChild()->toText();
	ASSERT_NE(firstChild, nullptr);
	EXPECT_EQ(firstChild->getText(), "third string,");
	EXPECT_EQ(firstChild->parent(), &root);
	EXPECT_EQ(firstChild->prev(), nullptr);
	EXPECT_EQ(firstChild->next()->next(), root.lastChild());
	EXPECT_EQ(root.lastChild()->prev()->prev(), root.firstChild());
}


TEST(XMLNodeTest, attributes) {
	XMLElement root("root");
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
	XMLElement root ("root");
	root.addAttribute("string", "text");
	root.addAttribute("integer", 42);
	root.addAttribute("double", 42.24);
	root.append("text");
	unique_ptr<XMLElement> clone = util::static_unique_ptr_cast<XMLElement>(root.clone());
	EXPECT_EQ(number_of_children(root), 1);
	EXPECT_STREQ(clone->getAttributeValue("string"), "text");
	EXPECT_STREQ(clone->getAttributeValue("integer"), "42");
	EXPECT_STREQ(clone->getAttributeValue("double"), "42.24");
}


TEST(XMLNodeTest, insertBefore) {
	XMLElement root("root");
	auto child1 = util::make_unique<XMLElement>("child1");
	auto child2 = util::make_unique<XMLElement>("child2");
	XMLNode* child1Ptr = root.append(std::move(child1));
	XMLNode* child2Ptr = root.append(std::move(child2));
	auto node = util::make_unique<XMLElement>("node");
	EXPECT_FALSE(root.insertBefore(util::make_unique<XMLElement>("dummy"), node.get()));
	EXPECT_EQ(number_of_children(root), 2);
	EXPECT_TRUE(root.insertBefore(util::make_unique<XMLElement>("child3"), child1Ptr));
	EXPECT_EQ(number_of_children(root), 3);
	XMLElement *child = root.firstChild()->toElement();
	EXPECT_EQ(child->name(), "child3");
	EXPECT_TRUE(root.insertBefore(util::make_unique<XMLElement>("child4"), child2Ptr));
	const char *names[] = {"child3", "child1", "child4", "child2"};
	const char **p = names;
	for (XMLNode *node=root.firstChild(); node; node=node->next()) {
		XMLElement *elem = node->toElement();
		ASSERT_NE(elem, nullptr);
		EXPECT_EQ(elem->name(), *p++) << "name=" << elem->name();
		EXPECT_EQ(elem->parent(), &root);
		if (elem->prev()) {
			EXPECT_EQ(elem->prev()->next(), elem);
		}
	}
}


TEST(XMLNodeTest, insertAfter) {
	XMLElement root("root");
	auto child1 = util::make_unique<XMLElement>("child1");
	auto child2 = util::make_unique<XMLElement>("child2");
	XMLNode *child1Ptr = root.append(std::move(child1));
	XMLNode *child2Ptr =root.append(std::move(child2));
	auto node = util::make_unique<XMLElement>("node");
	EXPECT_FALSE(root.insertAfter(util::make_unique<XMLElement>("dummy"), node.get()));
	EXPECT_EQ(number_of_children(root), 2);
	EXPECT_TRUE(root.insertAfter(util::make_unique<XMLElement>("child3"), child1Ptr));
	EXPECT_TRUE(root.insertAfter(util::make_unique<XMLElement>("child4"), child2Ptr));
	EXPECT_EQ(number_of_children(root), 4);
	const char *names[] = {"child1", "child3", "child2", "child4"};
	const char **p = names;
	for (XMLNode *node=root.firstChild(); node; node=node->next()) {
		XMLElement *elem = node->toElement();
		ASSERT_NE(elem, nullptr);
		EXPECT_EQ(elem->name(), *p++) << "name=" << elem->name();
		EXPECT_EQ(elem->parent(), &root);
		if (elem->prev()) {
			EXPECT_EQ(elem->prev()->next(), elem);
		}
	}
}


TEST(XMLNodeTest, getDescendants) {
	XMLElement root("root");
	auto child1 = util::make_unique<XMLElement>("child");
	auto child2 = util::make_unique<XMLElement>("childX");
	auto child3 = util::make_unique<XMLElement>("child");
	auto child4 = util::make_unique<XMLElement>("child");
	child1->addAttribute("attr", "value");
	child2->addAttribute("attr", "value");
	child3->addAttribute("attr", "value");
	child3->append("text");
	XMLElement *child3Ptr = static_cast<XMLElement*>(child2->append(std::move(child3)));
	XMLElement *child2Ptr = static_cast<XMLElement*>(child1->append(std::move(child2)));
	XMLElement *child1Ptr = static_cast<XMLElement*>(root.append(std::move(child1)));
	XMLElement *child4Ptr = static_cast<XMLElement*>(root.append(std::move(child4)));
	vector<XMLElement*> elements;
	root.getDescendants("child", nullptr, elements);
	EXPECT_EQ(elements.size(), 3u);
	{
		XMLElement *nodes[] = {child1Ptr, child3Ptr, child4Ptr};
		XMLElement **p = nodes;
		for (const XMLElement *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants("child", "attr", elements);
		EXPECT_EQ(elements.size(), 2u);
		XMLElement *nodes[] = {child1Ptr, child3Ptr};
		XMLElement **p = nodes;
		for (const XMLElement *elem : elements)
			EXPECT_EQ(elem, *p++);
	}{
		elements.clear();
		root.getDescendants(nullptr, "attr", elements);
		EXPECT_EQ(elements.size(), 3u);
		XMLElement *nodes[] = {child1Ptr, child2Ptr, child3Ptr};
		XMLElement **p = nodes;
		for (const XMLElement *elem : elements)
			EXPECT_EQ(elem, *p++);
	}
}


TEST(XMLNodeTest, getFirstDescendant) {
	XMLElement root("root");
	auto child1 = util::make_unique<XMLElement>("child");
	auto child2 = util::make_unique<XMLElement>("childX");
	auto child3 = util::make_unique<XMLElement>("child");
	auto child4 = util::make_unique<XMLElement>("child");
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
	XMLElement root("root");
	auto child1 = util::make_unique<XMLElement>("child");
	auto child2 = util::make_unique<XMLElement>("childX");
	auto child3 = util::make_unique<XMLElement>("child");
	auto child4 = util::make_unique<XMLElement>("child");
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
	XMLElement root("root");
	auto cdataNode = util::make_unique<XMLCData>("text & <text>");
	XMLNode *cdataNodePtr = root.append(std::move(cdataNode));
	root.append(util::make_unique<XMLElement>("element"));
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
