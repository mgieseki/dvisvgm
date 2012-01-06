/*************************************************************************
** PageSize.cpp                                                         **
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

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>
#include "PageSize.h"

using namespace std;

// make sure tolower is a function (and not a macro)
// so that 'transform' can be applied
static int my_tolower (int c) {
	return tolower(c);
}


/** Computes width and height of ISO/DIN An in millimeters.
 *  @param[in]  n the A level (e.g. n=4 => DIN A4)
 *  @param[out] width contains the page width when function returns
 *  @param[out] height contains the page height when function returns */
static void computeASize (int n, double &width, double &height) {
	double sqrt2 = sqrt(2.0);
	height = floor(1189.0/pow(sqrt2, n)+0.5);
	width  = floor(height/sqrt2+0.5);
}


/** Computes width and height of ISO/DIN Bn in millimeters.
 *  @param[in]  n the B level (e.g. n=4 => DIN B4)
 *  @param[out] width contains the page width when function returns
 *  @param[out] height contains the page height when function returns */
static void computeBSize (int n, double &width, double &height) {
	double w, h;
	computeASize(n, width, height);
	computeASize(n-1, w, h);
	width = floor(sqrt(width * w)+0.5);
	height = floor(sqrt(height * h)+0.5);
}


/** Computes width and height of ISO/DIN Cn in millimeters.
 *  @param[in] n the C level (e.g. n=4 => DIN C4)
 *  @param[out] width contains the page width when function returns
 *  @param[out] height contains the page height when function returns */
static void computeCSize (int n, double &width, double &height) {
	double w, h;
	computeASize(n, width, height);
	computeBSize(n, w, h);
	width = floor(sqrt(width * w)+0.5);
	height = floor(sqrt(height * h)+0.5);
}


/** Computes width and height of ISO/DIN Dn in millimeters.
 *  @param[in] n the D level (e.g. n=4 => DIN D4)
 *  @param[out] width contains the page width when function returns
 *  @param[out] height contains the page height when function returns */
static void computeDSize (int n, double &width, double &height) {
	double w, h;
	computeASize(n, width, height);
	computeBSize(n+1, w, h);
	width = floor(sqrt(width * w)+0.5);
	height = floor(sqrt(height * h)+0.5);
}


/** Constructs a PageSize object of given size.
 *  @param[in] name specifies the page size, e.g. "A4" or "letter" */
PageSize::PageSize (string name) : width(0), height(0) {
	resize(name);
}


void PageSize::resize (double w, double h) {
	width = w;
	height = h;
}

void PageSize::resize (string name) {
	if (name.length() < 2)
		throw PageSizeException("unknown page format: "+name);

	transform(name.begin(), name.end(), name.begin(), my_tolower);
	// extract optional suffix
	size_t pos = name.rfind("-");
	bool landscape = false;
	if (pos != string::npos) {
		string suffix = name.substr(pos);
		name = name.substr(0, pos);
		if (suffix == "-l" || suffix == "-landscape")
			landscape = true;
		else if (suffix != "-p" && suffix != "-portrait")
			throw PageSizeException("invalid page format suffix: " + suffix);
	}

	if (name == "invoice") {
		width = 140;
		height = 216;
	}
	else if (name == "executive") {
		width = 184;
		height = 267;
	}
	else if (name == "legal") {
		width = 216;
		height = 356;
	}
	else if (name == "letter") {
		width = 216;
		height = 279;
	}
	else if (name == "ledger") {
		width = 279;
		height = 432;
	}
	else if (isdigit(name[1]) && name.length() < 5) {  // limit length of number to prevent arithmetic errors
		istringstream iss(name.substr(1));
		int n;
		iss >> n;
		switch (name[0]) {
			case 'a' : computeASize(n, width, height); break;
			case 'b' : computeBSize(n, width, height); break;
			case 'c' : computeCSize(n, width, height); break;
			case 'd' : computeDSize(n, width, height); break;
			default  : throw PageSizeException("invalid page format: "+name);
		}
	}
	if (width == 0 || height == 0)
		throw PageSizeException("unknown page format: "+name);
	if (landscape)
		swap(width, height);

	const double ptpmm = 72.27/25.4; // TeX points per millimeter (72.27pt = 1in = 25.4mm)
	width *= ptpmm;
	height *= ptpmm;
}

