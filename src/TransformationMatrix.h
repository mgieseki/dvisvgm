/***********************************************************************
** TransformationMatrix.h                                             **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2006 Martin Gieseking <martin.gieseking@uos.de> **
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

#ifndef TRANSFORMATIONMATRIX_H
#define TRANSFORMATIONMATRIX_H

#include <istream>
#include <string>
#include "MessageException.h"
#include "Pair.h"

using std::istream;
using std::string;

struct ParserException : public MessageException
{
	ParserException (const string &msg) : MessageException(msg) {}
};

class Calculator;

class TransformationMatrix
{
   public:
		TransformationMatrix (const string &cmds, Calculator &calc);
		TransformationMatrix (double d=0);
      TransformationMatrix (double v[]);
		TransformationMatrix& parse (istream &is, Calculator &c);
		TransformationMatrix& parse (const string &cmds, Calculator &c);
		TransformationMatrix& rmultiply (const TransformationMatrix &tm);
		TransformationMatrix& translate (double tx, double ty);
		TransformationMatrix& scale (double sx, double sy);
		TransformationMatrix& rotate (double arc);
		TransformationMatrix& xskew (double arc);
		TransformationMatrix& yskew (double arc);
		TransformationMatrix& flip (bool h, double a);
		DPair operator * (const DPair &p) const;
		string getSVG () const;
		
   private:
		double values[3][3];
};

#endif
