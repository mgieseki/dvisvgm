/***********************************************************************
** XMLDocument.cpp                                                    **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/
// $Id$

#include "macros.h"
#include "XMLDocument.h"

using namespace std;

XMLDocument::XMLDocument (XMLElementNode *root)
	: rootElement(root), emitted(false) 
{
}

XMLDocument::~XMLDocument () {
	delete rootElement;
	FORALL(nodes, list<XMLNode*>::iterator, i) 
		delete *i;
}


void XMLDocument::append (XMLNode *node) {
	if (!node)
		return; 
	XMLElementNode *newRoot = dynamic_cast<XMLElementNode*>(node);
	if (newRoot) {             // there can only be one root element node in the document
		delete rootElement;     // so if there is already one...
		rootElement = newRoot;  // ...we replace it
	}
	else
		nodes.push_back(node);
}

void XMLDocument::setRootNode (XMLElementNode *root) {
	delete rootElement;
	rootElement = root;
}


ostream& XMLDocument::write (ostream &os) const {
	if (rootElement) { // no root element => no output
		os << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";
		FORALL(nodes, list<XMLNode*>::const_iterator, i) 
			(*i)->write(os);
		rootElement->write(os);
	}
	return os;
}

/** Writes a part of the XML document to the given output stream and removes
 *  the completely written nodes. The output stops when a stop node is reached
 *  (this node won't be printed at all). If a node was only partly emitted, i.e.
 *  its child was the stop node, a further call of emit will continue the output.
 *  @param os stream to which the output is sent to
 *  @param stopElement node where emitting stops (if 0 the whole tree will be emitted)
 *  @return true if node was completely emitted */
bool XMLDocument::emit (ostream& os, XMLNode *stopNode) {
	if (rootElement) {  // no root element => no output
		if (!emitted) {
			os << "<?xml version='1.0' encoding='ISO-8859-1'?>\n";
			emitted = true;
		}
		FORALL(nodes, list<XMLNode*>::iterator, i) {
			if ((*i)->emit(os, stopNode)) {
				list<XMLNode*>::iterator it = i++;  // prevent i from being invalidated...
				nodes.erase(it);              // ... by erase
				--i;  // @@ what happens if i points to first child?
			}
			else
				return false;
		}
		return rootElement->emit(os, stopNode);
	}
	return true;
}
