/***********************************************************************
** XMLNode.h                                                          **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef XMLNODE_H
#define XMLNODE_H

#include <list>
#include <map>
#include <ostream>
#include <string>

using std::list;
using std::map;
using std::ostream;
using std::string;

struct XMLNode
{
	virtual ~XMLNode () {}
	virtual ostream& write (ostream &os) const =0;
	virtual bool emit (ostream &os, XMLNode *stopElement);
	virtual void append (XMLNode *child) {}
	virtual void prepend (XMLNode *child) {}
};


class XMLElementNode : public XMLNode
{
	typedef map<string,string> AttribMap;
	typedef list<XMLNode*> ElementList;
	public:
      XMLElementNode (const string &name);
		~XMLElementNode ();
		void addAttribute (const string &name, const string &value);
		void addAttribute (const string &name, double value);
		void append (XMLNode *child);
		void append (const string &str);
		void prepend (XMLNode *child);
		bool hasAttribute (const string &name) const;
		ostream& write (ostream &os) const;
		bool emit (ostream &os, XMLNode *stopElement);
		bool empty () const  {return children.empty();}

	private:
		string name;          // element name (<name a1="v1" .. an="vn">...</name>)
		AttribMap attributes; 
		ElementList children; // child nodes
		bool emitted;         // true if node has been (partly) emitted
};


class XMLTextNode : public XMLNode
{
	public:
		XMLTextNode (const string &str) : text(str) {}
		void append (XMLNode *node);
		void append (XMLTextNode *node);
		void append (const string &str);
		void prepend (XMLNode *child);
		ostream& write (ostream &os) const {return os << text;}

	private:
		string text;
};


class XMLCommentNode : public XMLNode
{
	public:
		XMLCommentNode (const string &str) : text(str) {}
		ostream& write (ostream &os) const {return os << "<!--" << text << "-->\n";}

	private:
		string text;
};


class XMLDeclarationNode : public XMLNode 
{
	public:
		XMLDeclarationNode (const string &n, const string &p);
		~XMLDeclarationNode ();
		void append (XMLDeclarationNode *child);
		ostream& write (ostream &os) const;
		bool emit (ostream &os, XMLNode *stopElement);

	private:
		string name;
		string params;
		list<XMLDeclarationNode*> children;
		bool emitted;
};


class XMLCDataNode : public XMLNode
{
	public:
		XMLCDataNode (const string &d) : data(d) {}
		ostream& write (ostream &os) const;

	private:
		string data;
};

#endif
