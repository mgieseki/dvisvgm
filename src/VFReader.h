/***********************************************************************
** DVIReader.h                                                        **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
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
// $Id$

#ifndef VFREADER_H
#define VFREADER_H

#include <istream>
#include <map>
#include <stack>
#include <string>
#include "MessageException.h"
#include "StreamReader.h"
#include "types.h"

using std::istream;
using std::map;
using std::stack;
using std::string;

struct VFException : public MessageException
{
	VFException (const string &msg) : MessageException(msg) {}
};


class FontManager;

class VFReader : public StreamReader
{
	typedef bool (*ApproveAction)(int);
	public:
		VFReader (istream &is, FontManager *fm);
		virtual ~VFReader ();
		VFActions* replaceActions (VFActions *a);
		bool executeAll ();
		bool executePreambleAndFontDefs ();
		bool executeCharDefs ();

	protected:
		int executeCommand (ApproveAction approve=0);
//		UInt32 readUnsigned (int bytes);
//		Int32 readSigned (int bytes);
//		string readString (int length);
		UInt8* readBytes (int n, UInt8 *buf);
		
		// the following methods represent the VF commands 
		// they are called by executeCommand and should not be used directly
		void cmdPre ();
		void cmdPost ();
		void cmdShortChar (int pl);
		void cmdLongChar ();
		void cmdFontDef (int len);
		
	private:
		VFActions *actions;
		FontManager *fontManager;
};

#endif
