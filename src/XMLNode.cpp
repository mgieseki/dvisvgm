/*************************************************************************
** XMLNode.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include <map>
#include <list>
#include "macros.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;


XMLElementNode::XMLElementNode (const string &n) : _name(n) {
}


XMLElementNode::XMLElementNode (const XMLElementNode &node)
	: _name(node._name), _attributes(node._attributes)
{
	FORALL(node._children, ChildList::const_iterator, it)
		_children.push_back((*it)->clone());
}


XMLElementNode::~XMLElementNode () {
	FORALL(_children, ChildList::iterator, i)
		delete *i;
}


void XMLElementNode::clear () {
	_attributes.clear();
	FORALL(_children, ChildList::iterator, i)
		(*i)->clear();
}


void XMLElementNode::addAttribute (const string &name, const string &value) {
	_attributes[name] = value;
}


void XMLElementNode::addAttribute (const string &name, double value) {
	_attributes[name] = XMLString(value);
}


void XMLElementNode::append (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	if (!textNode1 || _children.empty())
		_children.push_back(child);
	else {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(_children.back()))
			textNode2->append(textNode1);  // merge two consecutive text nodes
		else
			_children.push_back(child);
	}
}


void XMLElementNode::append (const string &str) {
	if (_children.empty() || !dynamic_cast<XMLTextNode*>(_children.back()))
		_children.push_back(new XMLTextNode(str));
	else
		static_cast<XMLTextNode*>(_children.back())->append(str);
}


void XMLElementNode::prepend (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	if (!textNode1 || _children.empty())
		_children.push_front(child);
	else {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(_children.back()))
			textNode2->prepend(textNode1);  // merge two consecutive text nodes
		else
			_children.push_front(child);
	}
}


/** Inserts a new child node before a given child node already present. The latter
 *  will be the following sibling of the node to be inserted. If there's no such
 *  node present, nothing is inserted.
 *  @param[in] child node to be inserted
 *  @param[in] sibling following sibling of 'child'
 *  @return true on success */
bool XMLElementNode::insertBefore (XMLNode *child, XMLNode *sibling) {
	ChildList::iterator it = _children.begin();
	while (it != _children.end() && *it != sibling)
		++it;
	if (it != _children.end()) {
		_children.insert(it, child);
		return true;
	}
	return false;
}


/** Inserts a new child node after a given child node already present. The latter
 *  will be the preceding sibling of the node to be inserted. If there's no such
 *  node present, nothing is inserted.
 *  @param[in] child node to be inserted
 *  @param[in] sibling preceding sibling of 'child'
 *  @return true on success */
bool XMLElementNode::insertAfter (XMLNode *child, XMLNode *sibling) {
	ChildList::iterator it = _children.begin();
	while (it != _children.end() && *it != sibling)
		++it;
	if (it != _children.end()) {
		_children.insert(++it, child);
		return true;
	}
	return false;
}


/** Gets all descendant elements with a given name and given attribute.
 *  @param[in] name name of elements to find
 *  @param[in] attrName name of attribute to find
 *  @param[out] descendants all elements found
 *  @return true if at least one element was found  */
bool XMLElementNode::getDescendants (const char *name, const char *attrName, vector<XMLElementNode*> &descendants) const {
	FORALL(_children, ChildList::const_iterator, it) {
		if (XMLElementNode *elem = dynamic_cast<XMLElementNode*>(*it)) {
			if ((!name || elem->getName() == name) && (!attrName || elem->hasAttribute(attrName)))
				descendants.push_back(elem);
			elem->getDescendants(name, attrName, descendants);
		}
	}
	return !descendants.empty();
}


XMLElementNode* XMLElementNode::getFirstDescendant (const char *name, const char *attrName, const char *attrValue) const {
	FORALL(_children, ChildList::const_iterator, it) {
		if (XMLElementNode *elem = dynamic_cast<XMLElementNode*>(*it)) {
			if (!name || elem->getName() == name) {
				const char *value;
				if (!attrName || !attrValue || !(value = elem->getAttributeValue(attrName)) || string(value) == attrValue)
					return elem;
			}
			if (XMLElementNode *descendant = elem->getFirstDescendant(name, attrName, attrValue))
				return descendant;
		}
	}
	return 0;
}


ostream& XMLElementNode::write (ostream &os) const {
	os << '<' << _name;
	FORALL(_attributes, AttribMap::const_iterator, i)
		os << ' ' << i->first << "='" << i->second << '\'';
	if (_children.empty())
		os << "/>\n";
	else {
		os << '>';
		if (dynamic_cast<XMLElementNode*>(_children.front()))
			os << '\n';
		FORALL(_children, ChildList::const_iterator, i)
			(*i)->write(os);
		os << "</" << _name << ">\n";
	}
	return os;
}


/** Returns true if this element has an attribute of given name. */
bool XMLElementNode::hasAttribute (const string &name) const {
	return _attributes.find(name) != _attributes.end();
}


/** Returns the value of an attribute.
 *  @param[in] name name of attribute
 *  @return attribute value or 0 if attribute doesn't exist */
const char* XMLElementNode::getAttributeValue(const std::string& name) const {
	AttribMap::const_iterator it = _attributes.find(name);
	if (it != _attributes.end())
		return it->second.c_str();
	return 0;
}


//////////////////////

void XMLTextNode::append (XMLNode *node) {
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		append(tn);
	else
		delete node;
}


void XMLTextNode::append (XMLTextNode *node) {
	if (node)
		_text += node->_text;
	delete node;
}


void XMLTextNode::append (const string &str) {
	_text += str;
}


void XMLTextNode::prepend (XMLNode *node) {
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		prepend(tn);
	else
		delete node;
}


//////////////////////

XMLDeclarationNode::XMLDeclarationNode (const string &n, const string &p)
	: _name(n), _params(p)
{
}


XMLDeclarationNode::XMLDeclarationNode (const XMLDeclarationNode &node)
	: _name(node._name), _params(node._params)
{
	FORALL(node._children, list<XMLDeclarationNode*>::const_iterator, it)
		_children.push_back(new XMLDeclarationNode(**it));
}


XMLDeclarationNode::~XMLDeclarationNode () {
	FORALL(_children, list<XMLDeclarationNode*>::iterator, i)
		delete *i;
}


void XMLDeclarationNode::clear () {
	_params.clear();
	FORALL(_children, list<XMLDeclarationNode*>::iterator, i)
		(*i)->clear();
}


/** Appends another declaration node to this one.
 *  @param[in] child child to append, must be of type XMLDeclarationNode */
void XMLDeclarationNode::append (XMLNode *child) {
	if (XMLDeclarationNode *decl_node = dynamic_cast<XMLDeclarationNode*>(child))
		_children.push_back(decl_node);
	else
		delete child;
}


ostream& XMLDeclarationNode::write (ostream &os) const {
	os << "<!" << _name << ' ' << _params;
	if (_children.empty())
		os << ">\n";
	else {
		os << "[\n";
		FORALL(_children, list<XMLDeclarationNode*>::const_iterator, i)
			(*i)->write(os);
		os << "]>\n";
	}
	return os;
}

//////////////////////

ostream& XMLCDataNode::write (ostream &os) const {
	os << "<![CDATA[\n"
		<< _data
		<< "]]>\n";
	return os;
}


