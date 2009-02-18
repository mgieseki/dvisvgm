/***********************************************************************
** SpecialActions.h                                                   **
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

#ifndef SPECIALACTIONS_H
#define SPECIALACTIONS_H

#include <string>

class XMLElementNode;

struct SpecialActions
{
	virtual ~SpecialActions () {}
	virtual int getX() const =0;
	virtual int getY() const =0;
	virtual void setColor (const std::string &color) =0;
	virtual std::string getColor () const =0;
	virtual void appendInPage (XMLElementNode *node) =0;
};


struct SpecialEmptyActions : SpecialActions
{
	int getX() const {return 0;}
	int getY() const {return 0;}
	void setColor (const std::string &color) {}
	std::string getColor () const {return "";}
	void appendInPage (XMLElementNode *node) {}
};
#endif
