/***********************************************************************
** global.cpp                                                         **
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

#include "global.h"
#include <iostream>

using namespace std;

const double PI = 3.1415926535897932384626433832795;

void warning (char *fname, int line, char *msg) {
	cerr << "WARNING: " << fname << ", " << line << ": " << msg << endl;
}


void error (char *fname, int line, char *msg) {
	cerr << "ERROR: " << fname << ", " << line << ": " << msg << endl;
}


/*void show (char *fname, int line, char *expr, char *val) {
	cerr << fname << ", " << line << ": " << expr << "=" << val << endl;
}*/


void warning (char *fname, int line, const string &msg) {
	cerr << "WARNING: " << fname << ", " << line << ": " << msg << endl;
}


void error (char *fname, int line, const string &msg) {
	cerr << "ERROR: " << fname << ", " << line << ": " << msg << endl;
}

#ifdef __GNUC__
#include <cxxabi.h>

string demangle (const char *str) {
   int status;
   char *tmp = abi::__cxa_demangle(str, 0, 0, &status);
   string res;
   if (status == 0) {
      res = tmp;
      free(tmp);
      return res;
   }
   switch (status) {
      case -1: return "a memory allocation failiure occurred";
      case -2: return "ivalid name under the C++ ABI mangling rules";
      case -3: return "invalid argument(s)";
		default: return "unknown status code";
   }
}
#else
string demangle (const char *str) {
	return str;
}
#endif
