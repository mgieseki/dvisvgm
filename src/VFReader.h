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
// $Id: DVIReader.h 16 2007-01-05 20:28:01Z mgieseki $

#ifndef VFREADER_H
#define VFREADER_H

#include <istream>
#include <map>
#include <stack>
#include <string>
#include "MessageException.h"
#include "types.h"

using std::istream;
using std::map;
using std::stack;
using std::string;

struct VFException : public MessageException
{
	VFException (const string &msg) : MessageException(msg) {}
};

class FileFinder;
class FontInfo;

class VFReader
{
	public:
		VFReader (istream &is);
		virtual ~VFReader ();
		

	protected:
		int executeCommand ();
		UInt32 readUnsigned (int bytes);
		Int32 readSigned (int bytes);
		string readString (int length);
		const FileFinder* getFileFinder () const {return fileFinder;}
		
		// the following methods represent the VF commands 
		// they are called by executeCommand and should not be used directly
		void cmdNop (int len);
		void cmdBop (int len);
		void cmdEop (int len);
		void cmdPush (int len);
		void cmdPop (int len);
		void cmdRight (int len);
		void cmdDown (int len);
		void cmdFontDef (int len);
		void cmdFontNum0 (int n);
		void cmdFontNum (int len);
		void cmdXXX (int len);
		void cmdPre (int len);
		void cmdPost (int len);
		void cmdPostPost (int len);
		
	private:
		istream &in;
};

#endif
