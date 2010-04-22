/*************************************************************************
** XMLNode.h                                                            **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef XMLNODE_H
#define XMLNODE_H

#include <list>
#include <map>
#include <ostream>
#include <string>


struct XMLNode
{
	virtual ~XMLNode () {}
	virtual std::ostream& write (std::ostream &os) const =0;
	virtual bool emit (std::ostream &os, XMLNode *stopElement);
	virtual void append (XMLNode *child) {}
	virtual void prepend (XMLNode *child) {}
};


class XMLElementNode : public XMLNode
{
	typedef std::map<std::string,std::string> AttribMap;
	typedef std::list<XMLNode*> ElementList;
	public:
      XMLElementNode (const std::string &name);
		~XMLElementNode ();
		void addAttribute (const std::string &name, const std::string &value);
		void addAttribute (const std::string &name, double value);
		void append (XMLNode *child);
		void append (const std::string &str);
		void prepend (XMLNode *child);
		bool insertAfter (XMLNode *child, XMLNode *sibling);
		bool insertBefore (XMLNode *child, XMLNode *sibling);
		bool hasAttribute (const std::string &name) const;
		std::ostream& write (std::ostream &os) const;
		bool emit (std::ostream &os, XMLNode *stopElement);
		bool empty () const  {return children.empty();}

	private:
		std::string name;          // element name (<name a1="v1" .. an="vn">...</name>)
		AttribMap attributes;
		ElementList children; // child nodes
		bool emitted;         // true if node has been (partly) emitted
};


class XMLTextNode : public XMLNode
{
	public:
		XMLTextNode (const std::string &str) : text(str) {}
		void append (XMLNode *node);
		void append (XMLTextNode *node);
		void append (const std::string &str);
		void prepend (XMLNode *child);
		std::ostream& write (std::ostream &os) const {return os << text;}

	private:
		std::string text;
};


class XMLCommentNode : public XMLNode
{
	public:
		XMLCommentNode (const std::string &str) : text(str) {}
		std::ostream& write (std::ostream &os) const {return os << "<!--" << text << "-->\n";}

	private:
		std::string text;
};


class XMLDeclarationNode : public XMLNode
{
	public:
		XMLDeclarationNode (const std::string &n, const std::string &p);
		~XMLDeclarationNode ();
		void append (XMLDeclarationNode *child);
		std::ostream& write (std::ostream &os) const;
		bool emit (std::ostream &os, XMLNode *stopElement);

	private:
		std::string name;
		std::string params;
		std::list<XMLDeclarationNode*> children;
		bool emitted;
};


class XMLCDataNode : public XMLNode
{
	public:
		XMLCDataNode (const std::string &d) : data(d) {}
		std::ostream& write (std::ostream &os) const;

	private:
		std::string data;
};

#endif
