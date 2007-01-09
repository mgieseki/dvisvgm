/***********************************************************************
** global.h                                                           **
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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

using std::string;

#ifdef MESSAGES
  #define WARNING(msg) warning(__FILE__, __LINE__, msg)
  #define ERROR(msg)   error(__FILE__, __LINE__, msg)
#else
  #define WARNING(msg) 
  #define ERROR(msg) 
#endif

extern const double PI;

typedef unsigned char Byte;

void warning (char *fname, int line, const string &msg);
void warning (char *fname, int line, char *msg);
void error (char *fname, int line, const string &msg);
void error (char *fname, int line, char *msg);

string demangle (const char *str);

#endif

