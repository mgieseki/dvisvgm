/*************************************************************************
** Directory.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <config.h>
#include "Directory.hpp"

using namespace std;

#ifdef _WIN32
	#include <windows.h>
#else
	#include <errno.h>
	#include <sys/stat.h>
#endif


Directory::Directory () {
#if _WIN32
	_handle = INVALID_HANDLE_VALUE;
	_firstread = true;
	memset(&_fileData, 0, sizeof(WIN32_FIND_DATA));
#else
	_dir = 0;
	_dirent = 0;
#endif
}


Directory::Directory (const string &dirname) {
#if _WIN32
	_handle = INVALID_HANDLE_VALUE;
	_firstread = true;
	memset(&_fileData, 0, sizeof(WIN32_FIND_DATA));
#else
	_dir = 0;
	_dirent = 0;
#endif
	open(dirname);
}


Directory::~Directory () {
	close();
}


bool Directory::open (string dirname) {
	_dirname = dirname;
#ifdef _WIN32
	_firstread = true;
	if (dirname[dirname.length()-1] == '/' || dirname[dirname.length()-1] == '\\')
		dirname = dirname.substr(0, dirname.length()-1);
	dirname += "\\*";
	_handle = FindFirstFile(dirname.c_str(), &_fileData);
	return _handle != INVALID_HANDLE_VALUE;
#else
	_dir = opendir(_dirname.c_str());
	return bool(_dir);
#endif
}


void Directory::close () {
#ifdef _WIN32
	FindClose(_handle);
#else
	closedir(_dir);
#endif
}


/** Reads first/next directory entry.
 *  @param[in] type type of entry to return (a: file or dir, f: file, d: dir)
 *  @return name of entry */
const char* Directory::read (EntryType type) {
#ifdef _WIN32
	if (_handle == INVALID_HANDLE_VALUE)
		return 0;
	while (_firstread || FindNextFile(_handle, &_fileData)) {
		_firstread = false;
		if (_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (type == ET_FILE_OR_DIR || type == ET_DIR)
				return _fileData.cFileName;
		}
		else if (type == ET_FILE_OR_DIR || type == ET_FILE)
			return _fileData.cFileName;
	}
	FindClose(_handle);
	_handle = INVALID_HANDLE_VALUE;
	return 0;
#else
	if (!_dir)
		return 0;
	while ((_dirent = readdir(_dir))) {
		string path = string(_dirname) + "/" + _dirent->d_name;
		struct stat stats;
		if (stat(path.c_str(), &stats) == 0) {
			if (S_ISDIR(stats.st_mode)) {
				if (type == ET_FILE_OR_DIR || type == ET_DIR)
					return _dirent->d_name;
			}
			else if (type == ET_FILE_OR_DIR || type == ET_FILE)
				return _dirent->d_name;
		}
	}
	closedir(_dir);
	_dir = 0;
	return 0;
#endif
}


