/*************************************************************************
** Calculator.h                                                         **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
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

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <istream>
#include <map>
#include <string>
#include "MessageException.h"

using std::istream;
using std::map;
using std::string;

struct CalculatorException : public MessageException
{
	CalculatorException (const string &msg) : MessageException(msg) {}
};

class Calculator
{
   public:
      double eval (istream &is);
		double eval (const string &expr);
		void setVariable (const string &name, double value) {variables[name] = value;}
		double getVariable (const string &name) const;

   protected:
      double expr (istream &is, bool skip);
      double term (istream &is, bool skip);
      double prim (istream &is, bool skip);
      char lex (istream &is);
      char lookAhead (istream &is);

   private:
		map<string,double> variables;
		double numValue;
		string strValue;
};

#endif
