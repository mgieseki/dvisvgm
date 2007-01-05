/***********************************************************************
** FontMap.cpp                                                        **
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

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include "FontMap.h"

using namespace std;


static vector<string>& split (char *str, vector<string> &res, unsigned max) {
	if (str)
		while (*str && res.size() < max) {
			char *l = str;
			while (*l == ' ' || *l == '\t')  // skip spaces
				l++;
			if (*l && *l != '#') {  // begin of entry found?
				char *r = l;
				while (*r && *r != ' ' && *r != '\t')
					r++;
				str = r;
				if (*r) {		   // end of entry found?		
					*r = 0;        // mark end of entry
					str = r+1;     // next possible entry starts here
				}
				res.push_back(l);
			}
			else
				*str = 0;         // terminate
		}
	return res;
}


FontMap::FontMap (const string &fname, bool dir) {
	if (dir)
		readMapDir(fname);
	else
		readMapFile(fname);
}


#include <iostream>
bool FontMap::readMapFile (const string &fname) {
	char buf[256];
	ifstream ifs(fname.c_str());
	if (!ifs)
		return false;

	while (ifs) {
		ifs.getline(buf, 256);
		vector<string> columns;
		split(buf, columns, 2);
		if (columns.size() >= 2 && columns[0][0] != '#') 
			fontMap[columns[0]] = columns[1];
	}
	return true;
}


bool FontMap::readMapDir (const string &dirname) {
	DIR *dir = opendir(dirname.c_str());
	if (dir) {
		errno = 0;
		struct dirent *de;
		while ((de = readdir(dir)) != 0) {
			string fname = de->d_name;
			string path = string(dirname) + "/" + fname;
			struct stat stats;
			stat(path.c_str(), &stats);
			if (S_ISDIR(stats.st_mode) && fname.size() > 3 && fname.substr(fname.size()-4) == ".map")
				readMapFile(path);
			else if (errno != 0)
				return false;
		}
		closedir(dir);
		return true;
	}
	return false;
}


const char* FontMap::lookup (const string &fontname) const {
	ConstIterator it = fontMap.find(fontname);
	if (it == fontMap.end())
		return 0;
	return it->second.c_str();
}

