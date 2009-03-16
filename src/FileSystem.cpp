/***********************************************************************
** FileSystem.cpp                                                     **
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

#include <cstdlib>
#include <fstream>
#include "FileSystem.h"

#ifdef __GNUC__
#include <unistd.h>
#endif


using namespace std;

#ifdef __WIN32__
	#include <direct.h>
	#include <windows.h>
  	const char *FileSystem::DEVNULL = "nul";
  	const char FileSystem::PATHSEP = '\\';
	#define unlink _unlink
#else
	#include <pwd.h>
  	#include <sys/stat.h>
  	const char *FileSystem::DEVNULL = "/dev/null";
  	const char FileSystem::PATHSEP = '/';
#endif


bool FileSystem::remove (const string &fname) {
	return unlink(fname.c_str()) == 0;
}


bool FileSystem::rename (const string &oldname, const string &newname) {
	return ::rename(oldname.c_str(), newname.c_str()) == 0;
}


UInt64 FileSystem::filesize (const string &fname) {
#ifdef __WIN32__
	// unfortunately, stat doesn't work properly under Windows
	// so we have to use this freaky code
	WIN32_FILE_ATTRIBUTE_DATA attr;
	GetFileAttributesExA(fname.c_str(), GetFileExInfoStandard, &attr);
	return (static_cast<UInt64>(attr.nFileSizeHigh) << (8*sizeof(attr.nFileSizeLow))) | attr.nFileSizeLow;
#else
	struct stat attr;
	return (stat(fname.c_str(), &attr) == 0) ? attr.st_size : 0;
#endif
}

string FileSystem::adaptPathSeperators (string path) {
	for (unsigned i=0; i < path.length(); i++)
		if (path[i] == PATHSEP)
			path[i] = '/';
	return path;
}


string FileSystem::getcwd () {
	char buf[1024];
#ifdef __WIN32__
	return adaptPathSeperators(_getcwd(buf, 1024));
#else
	return ::getcwd(buf, 1024);
#endif
}



const char* FileSystem::userdir () {
#ifdef __WIN32__
	const char *drive=getenv("HOMEDRIVE");	
	const char *path=getenv("HOMEPATH");
	if (drive && path) {
		static string ret = string(drive)+path;
		if (!ret.empty())
			return ret.c_str();
	}
	return 0;
#else
	const char *dir=getenv("HOME");
	if (!dir) {
		if (const char *user=getenv("USER")) {
			if (struct passwd *pw=getpwnam(user))
				dir = pw->pw_dir;
		}
	}
	return dir;
#endif
}


bool FileSystem::mkdir (const char *dir) {
#ifdef __WIN32__
	int ret=_mkdir(dir);
#else
	int ret=::mkdir(dir, 0664);
#endif
	return ret == 0;
}


/** Checks if a file or directory exits. */
bool FileSystem::exists (const char *fname) {
#ifdef __WIN32__
	return GetFileAttributes(fname) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat attr;
	return stat(fname, &attr) == 0;
#endif
}
