/*************************************************************************
** AttributeExtractor.cpp                                               **
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
#include "AttributeExtractor.hpp"

using namespace std;

/** Constructs a new run object for an attribute and a given range [first, last) of nodes.
 *  @param[in] attr attribute to look for
 *  @param[in] first first node in sequence to consider
 *  @param[in] last first node after last node in sequence to consider */
AttributeExtractor::AttributeRun::AttributeRun (const Attribute &attr, Iterator first, Iterator last) {
	_length = 1;
	_begin = _end = first;
	for (++_end; _end != last; ++_end) {
		if ((*_end)->toText() || (*_end)->toCData())  // don't include text/CDATA nodes
			break;
		if (XMLElement *elem = (*_end)->toElement()) {
			if (!groupable(*elem))
				break;
			const char *val = elem->getAttributeValue(attr.name);
			if (val && val == attr.value)
				++_length;
			else
				break;
		}
	}
}


const char* AttributeExtractor::info () const {
	return "move common attributes from a sequence of elements to enclosing groups";
}


/** Performs the attribute extraction on a given context node. Each extracted
 *  attribute gets its own group, i.e. the extraction of multiple attributes
 *  of the same elements lead to nested groups.
 *  @param[in] context attributes of all children in this element are extracted
 *  @param[in] recurse if true, the algorithm is recursively performed on all descendant elements */
void AttributeExtractor::execute (XMLElement *context, bool recurse) {
	if (!context || context->children().empty())
		return;
	if (recurse) {
		for (auto &node : *context) {
			if (XMLElement *elem = node->toElement())
				execute(elem, true);
		}
	}
	for (auto it=context->begin(); it != context->end(); ++it)
		it = extractAttribute(it, *context);
}


/** Looks for the first attribute not yet processed and extracts it if possible.
 *  @param[in] pos points to the first node of a node sequence with potentially identical attributes
 *  @param[in] parent parent element whose child nodes are being processed
 *  @return iterator to group element if extraction was successful, 'pos' otherwise */
AttributeExtractor::Iterator AttributeExtractor::extractAttribute (Iterator pos, XMLElement &parent) {
	if (XMLElement *elem = (*pos)->toElement()) {
		for (const auto &currentAttribute : elem->attributes()) {
			if (!inheritable(currentAttribute) || extracted(currentAttribute))
				continue;
			AttributeRun run(currentAttribute, pos, parent.end());
			if (run.length() >= MIN_RUN_LENGTH) {
				XMLElement::Attribute attrib = currentAttribute;
				auto groupIt = parent.wrap(run.begin(), run.end(), "g");
				auto group = (*groupIt)->toElement();
				group->addAttribute(attrib.name, attrib.value);
				// remove attribute from the grouped elements but keep it on elements with 'id' attribute
				// since they can be referenced, and keep 'fill' attribute on animation elements
				for (auto &node : group->children()) {
					XMLElement *elem = node->toElement();
					if (elem && extractable(attrib, *elem))
						elem->removeAttribute(attrib.name);
				}
				// continue with children of the new group but ignore the just extracted attribute
				_extractedAttributes.insert(attrib.name);
				execute(group, false);
				_extractedAttributes.erase(attrib.name);
				return groupIt;
			}
		}
	}
	return pos;
}


/** Checks whether an element type is allowed to be put in a group element (<g>...</g>).
 *  For now we only consider a subset of the actually allowed set of elements.
 *  @param[in] elem name of element to check
 *  @return true if the element is groupable */
bool AttributeExtractor::groupable (const XMLElement &elem) {
	// https://www.w3.org/TR/SVG/struct.html#GElement
	static constexpr auto names = util::make_array(
		"a", "altGlyphDef", "animate", "animateColor", "animateMotion", "animateTransform",
		"circle", "clipPath", "color-profile", "cursor", "defs", "desc", "ellipse", "filter",
		"font", "font-face", "foreignObject", "g", "image", "line", "linearGradient", "marker",
		"mask", "path", "pattern", "polygon", "polyline", "radialGradient", "rect", "set",
		"style", "switch", "symbol", "text", "title", "use", "view"
	);
	return binary_search(names.begin(), names.end(), elem.getName(), [](const string &name1, const string &name2) {
		return name1 < name2;
	});
}


/** Checks whether an SVG attribute A of an element E implicitly propagates its properties
 *  to all child elements of E that don't specify A. For now we only consider a subset of
 *  the inheritable properties.
 *  @param[in] attrib name of attribute to check
 *  @return true if the attribute is inheritable */
bool AttributeExtractor::inheritable (const Attribute &attrib) {
	// subset of inheritable properties listed on https://www.w3.org/TR/SVG11/propidx.html
	// clip-path is not inheritable but can be moved to the parent element as long as
	// no child gets an different clip-path attribute
	// https://www.w3.org/TR/SVG11/styling.html#Inheritance
	static constexpr auto names = util::make_array(
		"clip-path", "clip-rule", "color", "color-interpolation", "color-interpolation-filters", "color-profile",
		"color-rendering", "direction", "fill", "fill-opacity", "fill-rule", "font", "font-family", "font-size",
		"font-size-adjust", "font-stretch", "font-style", "font-variant", "font-weight", "glyph-orientation-horizontal",
		"glyph-orientation-vertical", "letter-spacing", "paint-order", "stroke", "stroke-dasharray", "stroke-dashoffset",
		"stroke-linecap", "stroke-linejoin", "stroke-miterlimit", "stroke-opacity", "stroke-width", "transform",
		"visibility", "word-spacing", "writing-mode"
	);
	return binary_search(names.begin(), names.end(), attrib.name, [](const string &name1, const string &name2) {
		return name1 < name2;
	});
}


/** Checks whether an attribute is allowed to be removed from a given element. */
bool AttributeExtractor::extractable (const Attribute &attrib, XMLElement &element) {
	if (element.hasAttribute("id"))
		return false;
	if (attrib.name != "fill")
		return true;
	// the 'fill' attribute of animation elements has different semantics than
	// that of graphics elements => don't extract it from animation nodes
	// https://www.w3.org/TR/SVG11/animate.html#TimingAttributes
	static constexpr auto names = util::make_array(
		"animate", "animateColor", "animateMotion", "animateTransform", "set"
	);
	auto it = find_if(names.begin(), names.end(), [&](const string &name) {
		return element.getName() == name;
	});
	return it == names.end();
}


/** Returns true if a given attribute was already extracted from the
 *  current run of elements. */
bool AttributeExtractor::extracted (const Attribute &attr) const {
	return _extractedAttributes.find(attr.name) != _extractedAttributes.end();
}
