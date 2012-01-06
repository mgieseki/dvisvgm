/*************************************************************************
** XMLNode.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "macros.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;

bool XMLNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;
	write(os);
	return true;
}


XMLElementNode::XMLElementNode (const string &n) : _name(n), _emitted(false) {
}


XMLElementNode::~XMLElementNode () {
	FORALL(_children, ChildList::iterator, i)
		delete *i;
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


/** Writes a part of the XML tree to the given output stream and removes
 *  the completely written nodes. The output stops when a stop node is reached
 *  (this node won't be printed at all). If a node was only partly emitted, i.e.
 *  its child was the stop node, a further call of emit will continue the output.
 *  @param[in] os stream to which the output is sent to
 *  @param[in] stopElement node where emitting stops (if 0 the whole tree will be emitted)
 *  @return true if node was emitted completely */
bool XMLElementNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;

	if (!_emitted) {
		os << '<' << _name;
		FORALL(_attributes, AttribMap::iterator, i)
			os << ' ' << i->first << "='" << i->second << '\'';
		if (_children.empty())
			os << "/>\n";
		else {
			os << '>';
			if (dynamic_cast<XMLElementNode*>(_children.front()))
				os << '\n';
		}

		_emitted = true;
	}
	if (!_children.empty()) {
		FORALL(_children, ChildList::iterator, i) {
			if ((*i)->emit(os, stopNode)) {
				ChildList::iterator it = i++;  // prevent i from being invalidated...
				_children.erase(it);              // ... by erase
				--i;  // @@ what happens if i points to first child?
			}
			else
				return false;
		}
		os << "</" << _name << ">\n";
	}
	return true;
}


/** Returns true if this element has an attribute of given name. */
bool XMLElementNode::hasAttribute (const string &name) const {
	return _attributes.find(name) != _attributes.end();
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
	: _name(n), _params(p), _emitted(false)
{
}


XMLDeclarationNode::~XMLDeclarationNode () {
	FORALL(_children, list<XMLDeclarationNode*>::iterator, i)
		delete *i;
}

void XMLDeclarationNode::append (XMLDeclarationNode *child) {
	if (child)
		_children.push_back(child);
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


bool XMLDeclarationNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;

	if (!_emitted) {
		os << "<!" << _name << ' ' << _params;
		if (_children.empty())
			os << ">\n";
		else
			os << "[\n";
		_emitted = true;
	}
	if (!_children.empty()) {
		FORALL(_children, list<XMLDeclarationNode*>::iterator, i) {
			if ((*i)->emit(os, stopNode)) {
				list<XMLDeclarationNode*>::iterator it = i++;  // prevent i from being invalidated...
				_children.erase(it);              // ... by erase
				--i;  // @@ what happens if i points to first child?
			}
			else
				return false;
		}
		os << "]>\n";
	}
	return true;

}

//////////////////////

ostream& XMLCDataNode::write (ostream &os) const {
	os << "<![CDATA[\n"
		<< _data
		<< "]]>\n";
	return os;
}


