/***********************************************************************
** DVIReader.h                                                        **
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

#ifndef DVIREADER_H
#define DVIREADER_H

#include <map>
#include <stack>
#include <string>
#include "MessageException.h"
#include "StreamReader.h"
#include "VFActions.h"
#include "types.h"

using std::map;
using std::stack;
using std::string;

struct DVIException : public MessageException
{
	DVIException (const string &msg) : MessageException(msg) {}
};

struct InvalidDVIFileException : public DVIException
{
	InvalidDVIFileException(const string &msg) : DVIException(msg) {}
};

class DVIActions;
class FileFinder;
class FontManager;

class DVIReader : public StreamReader, protected VFActions
{
	struct DVIPosition
	{
		double h, v;
		double x, w, y, z;
		DVIPosition () {reset();}
		void reset ()  {h = v = x = w = y = z = 0.0;}
	};

	public:
		DVIReader (istream &is, DVIActions *a=0);
		virtual ~DVIReader ();
		
		bool executeDocument ();
		void executeAll ();
		bool executeAllPages ();
		void executePostamble ();
		bool executePage (unsigned n);
		bool isInPostamble () const               {return inPostamble;}
		double getXPos () const;
		double getYPos () const;
		double getPageWidth () const;
		double getPageHeight () const;
		int getCurrentFontNumber () const         {return currFontNum;}
		unsigned getTotalPages () const           {return totalPages;}
		DVIActions* getActions () const           {return actions;}
		DVIActions* replaceActions (DVIActions *a);

	protected:
		int executeCommand ();
		void putChar (UInt32 c, bool moveCursor);
		void defineFont (UInt32 fontnum, const string &name, UInt32 cs, double ds, double ss);
		const FontManager* getFontManager () const {return fontManager;}

		// VFAction methods
		void defineVFFont (UInt32 fontnum, string path, string name, UInt32 checksum, double dsize, double ssize);
		void defineVFChar (UInt32 c, vector<UInt8> *dvi);
		
		// the following methods represent the DVI commands 
		// they are called by executeCommand and should not be used directly
		void cmdSetChar0 (int c);
		void cmdSetChar (int len);
		void cmdPutChar (int len);
		void cmdSetRule (int len);
		void cmdPutRule (int len);
		void cmdNop (int len);
		void cmdBop (int len);
		void cmdEop (int len);
		void cmdPush (int len);
		void cmdPop (int len);
		void cmdRight (int len);
		void cmdDown (int len);
		void cmdX0 (int len);
		void cmdY0 (int len);
		void cmdW0 (int len);
		void cmdZ0 (int len);
		void cmdX (int len);
		void cmdY (int len);
		void cmdW (int len);
		void cmdZ (int len);
		void cmdFontDef (int len);
		void cmdFontNum0 (int n);
		void cmdFontNum (int len);
		void cmdXXX (int len);
		void cmdPre (int len);
		void cmdPost (int len);
		void cmdPostPost (int len);
		
	private:
		DVIActions *actions;
		bool inPage;         // true if between bop and eop
		UInt16 totalPages;   // total number of pages in dvi file
		int currFontNum;     // current font number
		double scaleFactor;  // 1 dvi unit = scaleFactor * TeX points
		UInt32 mag;          // magnification factor * 1000
		bool inPostamble;    // true if stream pointer is inside the postamble
		Int32 prevBop;       // pointer to previous bop
		double pageHeight, pageWidth;  // page height and width in TeX points
		DVIPosition currPos;
		stack<DVIPosition> posStack;
		FontManager *fontManager;
};

#endif
