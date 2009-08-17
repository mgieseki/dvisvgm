/*************************************************************************
** FileSystem.h                                                         **
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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include "types.h"

using std::string;

struct FileSystem
{
	static bool remove (const string &fname);
	static bool rename (const string &oldname, const string &newname);
	static UInt64 filesize (const string &fname);
	static string adaptPathSeperators (string path);
	static string getcwd ();
	static bool chdir (const char *dir);
	static bool exists (const char *file);
	static bool mkdir (const char *dir);
	static bool rmdir (const char *fname);
	static bool isDirectory (const char *fname);
	static bool isFile (const char *fname);
	static const char* userdir ();
	static const char* DEVNULL;
	static const char PATHSEP;
};

#endif
