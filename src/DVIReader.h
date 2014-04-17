/*************************************************************************
** DVIReader.h                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef DVISVGM_DVIREADER_H
#define DVISVGM_DVIREADER_H

#include <limits>
#include <map>
#include <stack>
#include <string>
#include "MessageException.h"
#include "StreamReader.h"
#include "VFActions.h"
#include "types.h"


struct DVIException : public MessageException
{
	DVIException (const std::string &msg) : MessageException(msg) {}
};

struct InvalidDVIFileException : public DVIException
{
	InvalidDVIFileException(const std::string &msg) : DVIException(msg) {}
};

struct DVIActions;
struct FileFinder;

class DVIReader : public StreamReader, protected VFActions
{
	typedef void (DVIReader::*CommandHandler)(int);
	enum DVIFormat {DVI_NONE=0, DVI_STANDARD=2, DVI_PTEX=3, DVI_XDV=5};
	enum WritingMode {WMODE_LR=0, WMODE_TB=1, WMODE_BT=3};

	struct DVIState
	{
		double h, v;        ///< horizontal and vertical cursor position
		double x, w, y, z;  ///< additional registers to store horizontal (x, w) and vertical (y, z) positions
		WritingMode d;      ///< direction: 0: horizontal, 1: vertical(top->bottom), 3: vertical (bottom->top)
		DVIState ()   {reset();}
		void reset () {h = v = x = w = y = z = 0.0; d=WMODE_LR;}
	};

	public:
		DVIReader (std::istream &is, DVIActions *a=0);

		bool executeDocument ();
		void executeAll ();
		bool executeAllPages ();
		void executePreamble ();
		void executePostamble ();
		bool executePage (unsigned n);
		bool executePages (unsigned first, unsigned last);
		bool inPostamble () const              {return _inPostamble;}
		double getXPos () const;
		double getYPos () const;
		void finishLine ()                     {_prevYPos = std::numeric_limits<double>::min();}
		void translateToX (double x)           {_tx=x-_dviState.h-_tx;}
		void translateToY (double y)           {_ty=y-_dviState.v-_ty;}
		double getPageWidth () const;
		double getPageHeight () const;
		int getStackDepth () const             {return _stateStack.size();}
		int getCurrentFontNumber () const      {return _currFontNum;}
		unsigned getCurrentPageNumber () const {return _currPageNum;}
		unsigned getTotalPages () const        {return _totalPages;}
		DVIActions* getActions () const        {return _actions;}
		DVIActions* replaceActions (DVIActions *a);

	protected:
		void verifyDVIFormat (int id) const;
		int executeCommand ();
		int evalCommand (bool compute_size, CommandHandler &handler, int &length, int &param);
		void moveRight (double dx);
		void moveDown (double dy);
		void putChar (UInt32 c, bool moveCursor);
		void putGlyphArray (bool xonly);
		void defineFont (UInt32 fontnum, const std::string &name, UInt32 cs, double ds, double ss);
		virtual void beginPage (unsigned pageno, Int32 *c) {}
		virtual void endPage (unsigned pageno) {}

		// VFAction methods
		void defineVFFont (UInt32 fontnum, std::string path, std::string name, UInt32 checksum, double dsize, double ssize);
		void defineVFChar (UInt32 c, std::vector<UInt8> *dvi);

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
		void cmdDir (int len);
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
		void cmdXPic (int len);
		void cmdXFontDef (int len);
		void cmdXGlyphA (int len);
		void cmdXGlyphS (int len);

	private:
		DVIFormat _dviFormat;    ///< format of DVI file currently processed
		DVIActions *_actions;    ///< actions to be performed on various DVI events
		bool _inPage;            ///< true if between bop and eop
		unsigned _totalPages;    ///< total number of pages in dvi file
		unsigned _currPageNum;   ///< current page number
		int _currFontNum;        ///< current font number
		double _dvi2bp;          ///< factor to convert dvi units to PS points
		UInt32 _mag;             ///< magnification factor * 1000
		bool _inPostamble;       ///< true if stream pointer is inside the postamble
		Int32 _prevBop;          ///< pointer to previous bop
		double _pageHeight, _pageWidth;  ///< page height and width in PS points
		DVIState _dviState;      ///< current cursor position
		std::stack<DVIState> _stateStack;
		double _prevYPos;        ///< previous vertical cursor position
		double _tx, _ty;         ///< tranlation of cursor position
		size_t _pageLength;      ///< number of bytes between current bop end eop
		std::streampos _pagePos; ///< distance of current DVI command from bop (in bytes)

	public:
		static bool COMPUTE_PAGE_LENGTH;  ///< if true, the bop handler computes the number of bytes of the current page
};

#endif
