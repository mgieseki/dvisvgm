/*************************************************************************
** XMLParser.hpp                                                        **
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

#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <string>
#include "MessageException.hpp"
#include "SVGTree.hpp"

struct XMLParserException : MessageException {
	explicit XMLParserException (const std::string &msg) : MessageException(msg) {}
};

class XMLParser {
	using AppendFunc = void (SVGTree::*)(std::unique_ptr<XMLNode>);
	using PushFunc = void (SVGTree::*)(std::unique_ptr<SVGElement>);
	using PopFunc = void (SVGTree::*)();
	using NameStack = std::vector<std::string>;

	public:
		XMLParser (AppendFunc append, PushFunc push, PopFunc pop)
				: _append(append), _pushContext(push), _popContext(pop) {}

		void parse (const std::string &xml, SVGTree &svgTree, bool finish=false);
		void finish (SVGTree &svgTree);

	protected:
		void openElement (const std::string &tag, SVGTree &svgTree);
		void closeElement (const std::string &tag, SVGTree &svgTree);

	private:
		AppendFunc _append;
		PushFunc _pushContext;
		PopFunc _popContext;
		std::string _xmlbuf;
		NameStack _nameStack;  ///< names of nested elements still missing a closing tag
		bool _error=false;
};

#endif
