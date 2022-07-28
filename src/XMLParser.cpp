/*************************************************************************
** XMLParser.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2022 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "InputReader.hpp"
#include "GraphicsPathParser.hpp"
#include "XMLParser.hpp"

using namespace std;

/** Parses a fragment of XML code, creates corresponding XML nodes and adds them
 *  to an SVG tree. The code may be split and processed by several calls of this
 *  function. Incomplete chunks that can't be processed yet are stored and picked
 *  up again together with the next incoming XML fragment. If a call of this function
 *  is supposed to finish the parsing of an XML subtree, parameter 'finish' must be set.
 *  @param[in] xml XML fragment to parse
 *  @param[in] svgTree the parsed nodes are added to this SVG tree
 *  @param[in] finish if true, no more XML is expected and parsing is finished */
void XMLParser::parse (const string &xml, SVGTree &svgTree, bool finish) {
	// collect/extract an XML fragment that only contains complete tags
	// incomplete tags are held back
	_xmlbuf += xml;
	string::size_type left=0;
	try {
		while (left != string::npos) {
			auto right = _xmlbuf.find('<', left);
			if (left < right && left < _xmlbuf.length())  // plain text found?
				(svgTree.*_append)(util::make_unique<XMLText>(_xmlbuf.substr(left, right - left)));
			if (right != string::npos) {
				left = right;
				if (_xmlbuf.compare(left, 9, "<![CDATA[") == 0) {
					right = _xmlbuf.find("]]>", left+9);
					if (right == string::npos) {
						if (finish) throw XMLParserException("expected ']]>' at end of CDATA block");
						break;
					}
					(svgTree.*_append)(util::make_unique<XMLCData>(_xmlbuf.substr(left + 9, right - left - 9)));
					right += 2;
				}
				else if (_xmlbuf.compare(left, 4, "<!--") == 0) {
					right = _xmlbuf.find("-->", left+4);
					if (right == string::npos) {
						if (finish) throw XMLParserException("expected '-->' at end of comment");
						break;
					}
					(svgTree.*_append)(util::make_unique<XMLComment>(_xmlbuf.substr(left + 4, right - left - 4)));
					right += 2;
				}
				else if (_xmlbuf.compare(left, 2, "<?") == 0) {
					right = _xmlbuf.find("?>", left+2);
					if (right == string::npos) {
						if (finish) throw XMLParserException("expected '?>' at end of processing instruction");
						break;
					}
					(svgTree.*_append)(util::make_unique<XMLText>(_xmlbuf.substr(left, right - left + 2)));
					right++;
				}
				else if (_xmlbuf.compare(left, 2, "</") == 0) {
					right = _xmlbuf.find('>', left+2);
					if (right == string::npos) {
						if (finish) throw XMLParserException("missing '>' at end of closing XML tag");
						break;
					}
					closeElement(_xmlbuf.substr(left+2, right-left-2), svgTree);
				}
				else {
					right = _xmlbuf.find('>', left+1);
					if (right == string::npos) {
						if (finish)	throw XMLParserException("missing '>' or '/>' at end of opening XML tag");
						break;
					}
					openElement(_xmlbuf.substr(left+1, right-left-1), svgTree);
				}
			}
			left = right;
			if (right != string::npos)
				left++;
		}
	}
	catch (const XMLParserException &e) {
		_error = true;
		throw;
	}
	if (left == string::npos)
		_xmlbuf.clear();
	else
		_xmlbuf.erase(0, left);
}


/** Processes an opening element tag.
 *  @param[in] tag tag without leading and trailing angle brackets */
void XMLParser::openElement (const string &tag, SVGTree &svgTree) {
	StringInputBuffer ib(tag);
	BufferInputReader ir(ib);
	string name = ir.getString("/ \t\n\r");
	bool isPathElement = (name == "path" || name == "svg:path");
	ir.skipSpace();
	auto elemNode = util::make_unique<SVGElement>(name);
	map<string, string> attribs;
	if (ir.parseAttributes(attribs, true, "\"'")) {
		for (const auto &attrpair : attribs) {
			if (!isPathElement || attrpair.first != "d")
				elemNode->addAttribute(attrpair.first, attrpair.second);
			else {
				try {
					// parse and reformat path definition
					auto path = GraphicsPathParser<double>().parse(attrpair.second);
					ostringstream oss;
					path.writeSVG(oss, SVGTree::RELATIVE_PATH_CMDS);
					elemNode->addAttribute("d", oss.str());
				}
				catch (const GraphicsPathParserException &e) {
					throw XMLParserException(string("error in path data: ")+e.what());
				}
			}
		}
	}
	ir.skipSpace();
	if (ir.peek() == '/')       // end of empty element tag
		(svgTree.*_append)(std::move(elemNode));
	else if (ir.peek() < 0) {   // end of opening tag
		_nameStack.push_back(name);
		(svgTree.*_pushContext)(std::move(elemNode));
	}
	else
		throw XMLParserException("'>' or '/>' expected at end of opening tag <"+name);
}


/** Processes a closing element tag.
 *  @param[in] tag tag without leading and trailing angle brackets */
void XMLParser::closeElement (const string &tag, SVGTree &svgTree) {
	StringInputBuffer ib(tag);
	BufferInputReader ir(ib);
	string name = ir.getString(" \t\n\r");
	ir.skipSpace();
	if (ir.peek() >= 0)
		throw XMLParserException("'>' expected at end of closing tag </"+name);
	if (_nameStack.empty())
		throw XMLParserException("spurious closing tag </" + name + ">");
	if (_nameStack.back() != name)
		throw XMLParserException("expected </" + _nameStack.back() + "> but found </" + name + ">");
	(svgTree.*_popContext)();
	_nameStack.pop_back();
}


/** Processes any remaining XML fragments, checks for missing closing tags,
 *  and resets the parser state. */
void XMLParser::finish (SVGTree &svgTree) {
	if (!_xmlbuf.empty()) {
		if (!_error)
			parse("", svgTree, true);
		_xmlbuf.clear();
	}
	string tags;
	while (!_nameStack.empty()) {
		tags += "</"+_nameStack.back()+">, ";
		_nameStack.pop_back();
	}
	if (!tags.empty() && !_error) {
		tags.resize(tags.length()-2);
		throw XMLParserException("missing closing tag(s): "+tags);
	}
}
