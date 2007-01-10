/***********************************************************************
** TransformationMatrix.cpp                                           **
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

#include <cmath>
#include <limits>
#include <sstream>
#include "Calculator.h"
#include "TransformationMatrix.h"

using namespace std;

static double deg2rad (double deg) {
   const double PI = acos(-1.0);
   return PI*deg/180.0;
}


static double round (double x, int n) {
   double pow10 = pow(10.0, n);
   return floor(x*pow10+0.5)/pow10;
}


TransformationMatrix::TransformationMatrix (double d) {
	for (int i=0; i < 3; i++)
		for (int j=0; j < 3; j++)
			values[i][j] = (i==j ? d : 0);
}


TransformationMatrix::TransformationMatrix (double v[]) {
	for (int i=0; i < 9; i++)
		values[i/3][i%3] = v[i];
}


TransformationMatrix::TransformationMatrix (const string &cmds, Calculator &calc) {
	parse(cmds, calc);
}


TransformationMatrix& TransformationMatrix::translate (double tx, double ty) {
	if (tx != 0 || ty != 0) {
		double v[] = {1, 0, tx, 0, 1, ty, 0, 0, 1};
		TransformationMatrix t(v);
		rmultiply(t);
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::scale (double sx, double sy) {
	if (sx != 1 || sy != 1) {
		double v[] = {sx, 0, 0, 0, sy, 0, 0, 0, 1};
		TransformationMatrix t(v);
		rmultiply(t);
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::rotate (double deg) {
   double rad = deg2rad(deg);
	double c = cos(rad);
	if (c != 1) {
		double s = sin(rad);
		double v[] = {c, -s, 0, s, c, 0, 0, 0, 1};
		TransformationMatrix t(v);
		rmultiply(t);
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::xskew (double deg) {
	double t = tan(deg2rad(deg));
	if (t != 0) {
		double v[] = {1, t, 0, 0, 1, 0, 0, 0, 1};
		TransformationMatrix t(v);
		rmultiply(t);
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::yskew (double deg) {
	double t = tan(deg2rad(deg));
	if (t != 0) {
		double v[] = {1, 0, 0, t, 1, 0, 0, 0, 1};
		TransformationMatrix t(v);
		rmultiply(t);
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::flip (bool haxis, double a) {
	double s = 1;
	if (haxis) // mirror at horizontal axis?
		s = -1;
	double v[] = {-s, 0, (haxis ? 0 : 2*a), 0, s, (haxis ? 2*a : 0), 0, 0, 1};
	TransformationMatrix t(v);
	rmultiply(t);
	return *this;
}


TransformationMatrix& TransformationMatrix::rmultiply (const TransformationMatrix &tm) {
	TransformationMatrix ret;
	for (int i=0; i < 3; i++)
		for (int j=0; j < 3; j++)
			for (int k=0; k < 3; k++)
				ret.values[i][j] += tm.values[i][k] * values[k][j];
	return *this = ret;
}


DPair TransformationMatrix::operator * (const DPair &p) const {
   double pp[] = {p.getX(), p.getY(), 1};
   double ret[]= {0, 0};
   for (int i=0; i < 2; i++)
      for (int j=0; j < 3; j++)
         ret[i] += values[i][j] * pp[j];
	return DPair(ret[0], ret[1]);
}


/** Gets a parameter for the transformation command.
 *  @param is parameter chars are read from this stream
 *  @param  calc parameters can be arithmetic expressions, so we need a calculator to evaluate them
 *  @param  def default value if parameter is optional
 *  @param  optional true if parameter is optional
 *  @param  leadingComma true first non-blank must be a comma
 *  @return value of argument */
static double getArgument (istream &is, Calculator &calc, double def, bool optional, bool leadingComma) {
	while (isspace(is.peek()))
		is.get();
	if (!optional && leadingComma && is.peek() != ',')
		throw ParserException("',' expected");
   if (is.peek() == ',') {
   	is.get();         // skip comma
      optional = false; // now we expect a parameter
   }
	string expr;
	while (is && !isupper(is.peek()) && is.peek() != ',')
		expr += is.get();
   if (expr.length() == 0)
      if (optional)
         return def;
      else
         throw ParserException("parameter expected");
	return calc.eval(expr);
}


TransformationMatrix& TransformationMatrix::parse (istream &is, Calculator &calc) {
	*this = TransformationMatrix(1);
	while (is) {
      while (isspace(is.peek()))
         is.get();
		char cmd = is.get();
		switch (cmd) {
			case 'T': {
				double tx = getArgument(is, calc, 0, false, false);
				double ty = getArgument(is, calc, 0, false, true);
				translate(tx, ty);
            break;
			}
			case 'S': {
				double sx = getArgument(is, calc, 1, false, false);
				double sy = getArgument(is, calc, sx, true, true );
				scale(sx, sy);
            break;
			}
			case 'R': {
				double a = getArgument(is, calc, 0, false, false);
				double x = getArgument(is, calc, calc.getVariable("ux")+calc.getVariable("w")/2, true, true);
				double y = getArgument(is, calc, calc.getVariable("uy")+calc.getVariable("h")/2, true, true);
				translate(-x, -y);
				rotate(a);
				translate(x, y);
            break;
			}
			case 'F': {
				char c = is.get();
				if (c != 'H' && c != 'V')
					throw ParserException("'H' or 'V' expected");
				double a = getArgument(is, calc, 0, false, false);
				flip(c == 'H', a);
				break;
			}
			case 'K': {
				char c = is.get();
				if (c != 'X' && c != 'Y')
					throw ParserException("transformation command 'K' must be followed by 'X' or 'Y'");
				double a = getArgument(is, calc, 0, false, false);
				if (fabs(cos(deg2rad(a))) <= numeric_limits<double>::epsilon()) {
					ostringstream oss;
					oss << "illegal skewing angle: " << a << " degrees";
					throw ParserException(oss.str());
				}
				if (c == 'X')
					xskew(a);
				else
					yskew(a);
            break;
			}
			case 'M': {
				double v[6];
				for (int i=0; i < 6; i++)
					v[i] = getArgument(is, calc, i%4 ? 0 : 1, i==0, i==0);
				for (int i=6; i < 9; i++)
					v[i] = i%4 ? 0 : 1;
				TransformationMatrix tm(v);
				rmultiply(tm);
            break;
			}
         default:
				ostringstream oss;
				oss << "transformation command expected (found '" << cmd << "' instead)";
            throw ParserException(oss.str());
		}
	}
	return *this;
}


TransformationMatrix& TransformationMatrix::parse (const string &cmds, Calculator &calc) {
   istringstream iss;
   iss.str(cmds);
   return parse(iss, calc);
}


string TransformationMatrix::getSVG () const {
	ostringstream oss;
	for (int i=0; i < 3; i++)
		for (int j=0; j < 2; j++)
			oss << ' ' << round(values[j][i], 3);
	return "matrix(" + oss.str().substr(1) + ")";
}


#if 0
#include <iostream>

int main (int argc, char **argv) {
   Calculator calc;
   TransformationMatrix tm;
   try {
      tm.parse(argv[1], calc);
   }
   catch (CalculatorException e) {
      cout << "calc: " << e.getMessage() << endl;
   }
   catch (ParserException e) {
      cout << "parser: " << e.getMessage() << endl;
   }
   cout << tm.getSVG() << endl;
}
#endif
