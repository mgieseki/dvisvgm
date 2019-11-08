/*************************************************************************
** TextSimplifier.cpp                                                   **
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

#include "TextSimplifier.hpp"
#include "../XMLNode.hpp"

const char* TextSimplifier::info () const {
	return "merge single tspans into surrounding text elements";
}

/** Checks if there's only a single tspan element and optional whitespace
 *  siblings inside a given text element.
 *  @param[in] textElement text element to check
 *  @return pointer to the only tspan element or nullptr */
static XMLElement* only_tspan_child (XMLElement *textElement) {
	XMLElement *tspan=nullptr;
	for (XMLNode *child : *textElement) {
		if (child->toWSNode() || child->toComment())
			continue;
		if (child->toText())
			return nullptr;
		if (XMLElement *childElement = child->toElement()) {
			if (tspan || childElement->name() != "tspan")
				return nullptr;
			tspan = childElement;
		}
	}
	return tspan;
}


void TextSimplifier::execute (XMLElement *context) {
	if (!context)
		return;
	if (context->name() == "text") {
		if (XMLElement *tspan = only_tspan_child(context)) {
			// move all attributes of the tspan to the text element and unwrap the tspan
			for (const XMLElement::Attribute &attr : tspan->attributes())
				context->addAttribute(attr.name, attr.value);
			XMLElement::unwrap(tspan);
		}
	}
	else {
		XMLNode *node = context->firstChild();
		while (node) {
			XMLNode *next = node->next();  // keep safe pointer to next node
			if (XMLElement *elem = node->toElement())
				execute(elem);
			node = next;
		}
	}
}
