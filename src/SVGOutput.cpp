/*************************************************************************
** SVGOutput.cpp                                                        **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "gzstream.h"
#include "FileSystem.h"
#include "MessageException.h"
#include "SVGOutput.h"

using namespace std;

SVGOutput::SVGOutput (const char *base, string pattern, int zipLevel)
	: _path(base ? base : ""),
	_pattern(pattern),
	_stdout(base == 0),
	_zipLevel(zipLevel),
	_page(-1),
	_os(0)
{
}


/** Returns an output stream for the given page.
 *  @param[in] page number of current page
 *  @param[in] numPages total number of pages in the DVI file
 *  @return output stream for the given page */
ostream& SVGOutput::getPageStream (int page, int numPages) const {
	string fname = filename(page, numPages);
	if (fname.empty()) {
		delete _os;
		_os = 0;
		return cout;
	}
	if (page == _page)
		return *_os;

	_page = page;
	delete _os;

	if (_zipLevel > 0)
		_os = new ogzstream(fname.c_str(), _zipLevel);
	else
		_os = new ofstream(fname.c_str());
	if (!_os || !*_os) {
		delete _os;
		_os = 0;
		throw MessageException("can't open file "+fname+" for writing");
	}
	return *_os;
}


/** Returns the name of the SVG file containing the given page.
 *  @param[in] page number of current page
 *  @param[in] numPages total number of pages */
string SVGOutput::filename (int page, int numPages) const {
	if (_stdout)
		return "";
	string fname = _pattern;
	if (fname.empty())
		fname = numPages > 1 ? "%f-%p" : "%f";
	else if (numPages > 1 && fname.find("%p") == string::npos)
		fname += FileSystem::isDirectory(fname.c_str()) ? "/%f-%p" : "-%p";

	// replace pattern variables by their actual values
	// %f: basename of the DVI file
	// %p: current page number
	ostringstream oss;
	oss << setfill('0') << setw(max(2, int(1+log10((double)numPages)))) << page;
	size_t pos=0;
	while ((pos = fname.find('%', pos)) != string::npos && pos < fname.length()-1) {
		switch (fname[pos+1]) {
			case 'f': fname.replace(pos, 2, _path.basename());  pos += _path.basename().length(); break;
			case 'p': fname.replace(pos, 2, oss.str()); pos += oss.str().length(); break;
			default : ++pos;
		}
	}
	FilePath outpath(fname, true);
	if (outpath.suffix().empty())
		outpath.suffix(_zipLevel > 0 ? "svgz" : "svg");
	string apath = outpath.absolute();
	string rpath = outpath.relative();
	return apath.length() < rpath.length() ? apath : rpath;
}
