/*************************************************************************
** Directory.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "Directory.h"

using namespace std;

#ifdef __WIN32__
	#include <windows.h>
#else
	#include <errno.h>
	#include <sys/stat.h>
#endif


Directory::Directory () {
#if __WIN32__
	handle = INVALID_HANDLE_VALUE;
#else
	dir = 0;
#endif
}


Directory::Directory (string dirname) {
	open(dirname);
}

Directory::~Directory () {
	close();
}


bool Directory::open (string dname) {
	_dirname = dname;
#ifdef __WIN32__
	firstread = true;
	if (dname[dname.length()-1] == '/' || dname[dname.length()-1] == '\\')
		dname = dname.substr(0, dname.length()-1);
	dname += "\\*";
	handle = FindFirstFile(dname.c_str(), &fileData);
	return handle != INVALID_HANDLE_VALUE;
#else
	dir = opendir(_dirname.c_str());
	return dir;
#endif
}


void Directory::close () {
#ifdef __WIN32__
	FindClose(handle);
#else
	closedir(dir);
#endif
}


/** Reads first/next directory entry.
 *  @param[in] type type of entry to return (a: file or dir, f: file, d: dir)
 *  @return name of entry */
const char* Directory::read (char type) {
#ifdef __WIN32__
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	while (firstread || FindNextFile(handle, &fileData)) {
		firstread = false;
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (type == 'a' || type == 'd')
				return fileData.cFileName;
		}
		else if (type == 'a' || type == 'f')
			return fileData.cFileName;
	}
	FindClose(handle);
	handle = INVALID_HANDLE_VALUE;
	return 0;
#else
	if (!dir)
		return 0;
	while ((dirent = readdir(dir))) {
		string path = string(_dirname) + "/" + dirent->d_name;
		struct stat stats;
		stat(path.c_str(), &stats);
		if (S_ISDIR(stats.st_mode)) {
			if (type == 'a' || type == 'd')
				return dirent->d_name;
		}
		else if (type == 'a' || type == 'f')
			return dirent->d_name;
	}
	closedir(dir);
	dir = 0;
	return 0;
#endif
}


