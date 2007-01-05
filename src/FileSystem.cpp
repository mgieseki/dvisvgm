/***********************************************************************
** FileSystem.cpp                                                     **
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

#include <fstream>
#include "FileSystem.h"

#ifdef __GNUC__
#include <unistd.h>
#endif


using namespace std;


#ifdef __WIN32__
	#include <windows.h>
  	const char *FileSystem::DEVNULL = "nul";
  	const char FileSystem::PATHSEP = '\\';
	#define unlink _unlink
#else
  	#include <sys/stat.h>
  	const char *FileSystem::DEVNULL = "/dev/null";
  	const char FileSystem::PATHSEP = '/';
#endif


bool FileSystem::remove (string fname) {
	return unlink(fname.c_str()) == 0;
}


bool FileSystem::rename (string oldname, string newname) {
	return ::rename(oldname.c_str(), newname.c_str()) == 0;
}


UInt64 FileSystem::filesize (string fname) {
#ifdef __WIN32__
	// unfortunately, stat doesn't work properly under Windows
	// so we have to use this freaky code
	WIN32_FILE_ATTRIBUTE_DATA attr;
	GetFileAttributesExA(fname.c_str(), GetFileExInfoStandard, &attr);
	return (static_cast<UInt64>(attr.nFileSizeHigh) << (8*sizeof(attr.nFileSizeLow))) | attr.nFileSizeLow;
#else
	struct stat attr;
	stat(fname.c_str(), &attr);
	return attr.st_size;
#endif
}

string FileSystem::adaptPathSeperators (string path) {
	for (unsigned i=0; i < path.length(); i++)
		if (path[i] == PATHSEP)
			path[i] = '/';
	return path;
}
