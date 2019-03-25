/*************************************************************************
** GroupCollapser.cpp                                                   **
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

#include <algorithm>
#include <array>
#include <string>
#include <vector>
#include "AttributeExtractor.hpp"
#include "GroupCollapser.hpp"
#include "../XMLNode.hpp"

using namespace std;

const char* GroupCollapser::info () const {
	return "join nested group elements";
}


/** Checks if the only children of a given element are a single element and
 *  optional whitespace nodes.
 *  @param[in] elem element to check
 *  @return iterator pointing to the only child element or elem->end() */
static XMLElement::Iterator only_child_element (XMLElement *elem) {
	auto childIt = elem->end();
	for (auto it=elem->begin(); it != elem->end(); ++it) {
		if ((*it)->toElement()) {
			if (childIt != elem->end())
				return elem->end();
			childIt = it;
		}
		else if (!(*it)->toWSNode())
			return elem->end();
	}
	return childIt;
}


/** Removes all whitespace nodes from a given element. */
static void remove_ws_nodes (XMLElement *elem) {
	auto it=elem->begin();
	while (it != elem->end()) {
		if ((*it)->toWSNode())
			it = elem->remove(it);
		else
			++it;
	}
}


/** Recursively removes all redundant group elements from the given context element
 *  and moves the attributes to the corresponding parent element. */
void GroupCollapser::execute (XMLElement *context) {
	if (!context)
		return;
	for (auto &node : *context) {
		if (XMLElement *elem = node->toElement())
			execute(elem);
	}
	auto it = only_child_element(context);
	if (it != context->end() && collapsible(*context)) {
		XMLElement *child = (*it)->toElement();
		if (child->getName() == "g" && unwrappable(*child, *context) && moveAttributes(*child, *context)) {
			remove_ws_nodes(context);
			context->unwrap(context->begin());
		}
	}
}


/** Moves all attributes from an element to another one. The attributes are
 *  removed from the source. Attributes already present in the destination
 *  element are overwritten or combined.
 *  @param[in] source element the attributes are taken from
 *  @param[in] dest element that receives the attributes
 *  @return true if all attributes have been moved */
bool GroupCollapser::moveAttributes (XMLElement &source, XMLElement &dest) {
	vector<string> movedAttributes;
	for (const XMLElement::Attribute &attr : source.attributes()) {
		if (attr.name == "transform") {
			string transform;
			if (const char *destvalue = dest.getAttributeValue("transform"))
				transform = destvalue+attr.value;
			else
				transform = attr.value;
			dest.addAttribute("transform", transform);
			movedAttributes.emplace_back("transform");
		}
		else if (AttributeExtractor::inheritable(attr)) {
			dest.addAttribute(attr.name, attr.value);
			movedAttributes.emplace_back(attr.name);
		}
	}
	for (const string &attrname : movedAttributes)
		source.removeAttribute(attrname);
	return source.attributes().empty();
}


/** Returns true if a given element is allowed to take the inheritable attributes
 *  and children of a child group without changing the semantics.
 *  @param[in] element group element to check */
bool GroupCollapser::collapsible (const XMLElement &element) {
	// the 'fill' attribute of animation elements has different semantics than
	// that of graphics elements => don't collapse them
	static constexpr auto names = util::make_array(
		"animate", "animateColor", "animateMotion", "animateTransform", "set"
	);
	auto it = find_if(names.begin(), names.end(), [&](const string &name) {
		return element.getName() == name;
	});
	return it == names.end();
}


/** Returns true if a given group element is allowed to be unwrapped, i.e. its
 *  attributes and children can be moved to the parent without changing the semantics.
 *  @param[in] source element whose children and attributes should be moved
 *  @param[in] dest element that should receive the children and attributes */
bool GroupCollapser::unwrappable (const XMLElement &source, const XMLElement &dest) {
	// check for colliding clip-path attributes
	if (const char *cp1 = source.getAttributeValue("clip-path")) {
		if (const char *cp2 = dest.getAttributeValue("clip-path")) {
			if (string(cp1) != cp2)
				return false;
		}
	}
	// these attributes prevent a group from being unwrapped
	static constexpr auto attribs = util::make_array(
		"class", "id", "filter", "mask", "style"
	);
	auto it = find_if(attribs.begin(), attribs.end(), [&](const string &name) {
		return source.hasAttribute(name) || dest.hasAttribute(name);
	});
	return it == attribs.end();
}
