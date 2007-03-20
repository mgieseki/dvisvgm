/************************************************
* Directory.cpp                                *
*                                               *
* This file is part of dvisvgm                  *
* Copyright (c) 2005 by Martin Gieseking        *
************************************************/
// $Id$

#include "Directory.h"
#include "macros.h"

using namespace std;

#ifdef __WIN32__
#include <windows.h>
#endif


Directory::Directory () {
	handle = INVALID_HANDLE_VALUE;
}


Directory::Directory (const string &dirname) {
	open(dirname);
}

Directory::~Directory () {
	close();
}


bool Directory::open (const string &dirname) {
	firstread = true;
#ifdef __WIN32__
	handle = FindFirstFile(dirname.c_str(), &fileData);
	return handle != INVALID_HANDLE_VALUE;
#else
	dir = opendir(dirname.c_str());
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
	if (dirent = readdir(dir))
		return dirent->d_name;
	return 0;
#endif
}

#if 0
#include <iostream>
int main () {
	Directory dir("c:/develop/*");
	while (const char *name = dir.read('d'))
		cout << name << endl;
}	
#endif
