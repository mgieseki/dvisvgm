/*************************************************************************
** XMLDocTypeNode.h                                                     **
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

#ifndef XMLDOCTYPENODE_H
#define XMLDOCTYPENODE_H

#include "XMLNode.h"

class XMLDocTypeNode : public XMLDeclarationNode
{
   public:
      XMLDocTypeNode (const std::string &rootName, const std::string &type, const std::string &param)
			: XMLDeclarationNode("DOCTYPE", rootName + " " + type + " " + param) {}
};


class XMLEntityNode : public XMLDeclarationNode
{
	public:
		XMLEntityNode (const std::string &n, const std::string &v)
			: XMLDeclarationNode("ENTITY", n + " " + v) {}
};

#endif
