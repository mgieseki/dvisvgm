/*************************************************************************
** XMLNode.hpp                                                          **
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

#ifndef XMLNODE_HPP
#define XMLNODE_HPP

#include <deque>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include "utility.hpp"


class XMLNode {
	public:
		virtual ~XMLNode () =default;
		virtual std::unique_ptr<XMLNode> clone () const =0;
		virtual void clear () =0;
		virtual std::ostream& write (std::ostream &os) const =0;
};


class XMLElement : public XMLNode {
	public:
		struct Attribute {
			Attribute (const std::string &nam, const std::string &val) : name(nam), value(val) {}
			std::string name;
			std::string value;
		};
		using ChildList = std::deque<std::unique_ptr<XMLNode>>;
		static bool WRITE_NEWLINES;  ///< insert line breaks after element tags

	public:
		XMLElement (const std::string &name);
		XMLElement (const XMLElement &node);
		XMLElement (XMLElement &&node);
		std::unique_ptr<XMLNode> clone () const override {return util::make_unique<XMLElement>(*this);}
		void clear () override;
		void addAttribute (const std::string &name, const std::string &value);
		void addAttribute (const std::string &name, double value);
		XMLNode* append (std::unique_ptr<XMLNode> &&child);
		XMLNode* append (const std::string &str);
		XMLNode* prepend (std::unique_ptr<XMLNode> &&child);
		void remove (const XMLNode *child);
		bool insertAfter (std::unique_ptr<XMLNode> &&child, XMLNode *sibling);
		bool insertBefore (std::unique_ptr<XMLNode> &&child, XMLNode *sibling);
		bool hasAttribute (const std::string &name) const;
		const char* getAttributeValue (const std::string &name) const;
		bool getDescendants (const char *name, const char *attrName, std::vector<XMLElement*> &descendants) const;
		XMLElement* getFirstDescendant (const char *name, const char *attrName, const char *attrValue) const;
		std::ostream& write (std::ostream &os) const override;
		bool empty () const                  {return _children.empty();}
		const ChildList& children () const   {return _children;}
		const std::string& getName () const  {return _name;}

	protected:
		Attribute* getAttribute (const std::string &name);
		const Attribute* getAttribute (const std::string &name) const;

	private:
		std::string _name;     // element name (<name a1="v1" .. an="vn">...</name>)
		std::vector<Attribute> _attributes;
		ChildList _children;   // child nodes
};


class XMLText : public XMLNode {
	public:
		XMLText (const std::string &str) : _text(str) {}
		XMLText (std::string &&str) : _text(std::move(str)) {}
		std::unique_ptr<XMLNode> clone () const override {return util::make_unique<XMLText>(*this);}
		void clear () override {_text.clear();}
		void append (std::unique_ptr<XMLNode> &&node);
		void append (std::unique_ptr<XMLText> &&node);
		void append (const std::string &str);
		void prepend (std::unique_ptr<XMLNode> &&node);
		std::ostream& write (std::ostream &os) const override {return os << _text;}
		const std::string& getText () const {return _text;}

	private:
		std::string _text;
};


class XMLComment : public XMLNode {
	public:
		XMLComment (const std::string &str) : _text(str) {}
		XMLComment (std::string &&str) : _text(std::move(str)) {}
		std::unique_ptr<XMLNode> clone () const override {return util::make_unique<XMLComment>(*this);}
		void clear () override {_text.clear();}
		std::ostream& write (std::ostream &os) const override {return os << "<!--" << _text << "-->";}

	private:
		std::string _text;
};


class XMLCData : public XMLNode {
	public:
		XMLCData () =default;
		XMLCData (const std::string &d) : _data(d) {}
		XMLCData (std::string &&d) : _data(std::move(d)) {}
		std::unique_ptr<XMLNode> clone () const override {return util::make_unique<XMLCData>(*this);}
		void clear () override                {_data.clear();}
		void append (std::string &&str);
		std::ostream& write (std::ostream &os) const override;

	private:
		std::string _data;
};


inline std::ostream& operator << (std::ostream &os, const XMLElement &node) {return node.write(os);}
inline std::ostream& operator << (std::ostream &os, const XMLText &node) {return node.write(os);}
inline std::ostream& operator << (std::ostream &os, const XMLComment &node) {return node.write(os);}
inline std::ostream& operator << (std::ostream &os, const XMLCData &node) {return node.write(os);}

#endif
