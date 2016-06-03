/*************************************************************************
** BgColorSpecialHandler.cpp                                            **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <algorithm>
#include "BgColorSpecialHandler.h"
#include "ColorSpecialHandler.h"
#include "SpecialActions.h"

using namespace std;


/** Collect all background color changes while preprocessing the DVI file.
 *  We need them in order to apply the correct background colors even if
 *  not all but only selected DVI pages are converted. */
void BgColorSpecialHandler::preprocess (const char*, std::istream &is, SpecialActions &actions) {
	Color color = ColorSpecialHandler::readColor(is);
	unsigned pageno = actions.getCurrentPageNumber();
	if (_pageColors.empty() || _pageColors.back().second != color) {
		if (!_pageColors.empty() && _pageColors.back().first == pageno)
			_pageColors.back().second = color;
		else
			_pageColors.push_back(PageColor(pageno, color));
	}
}


bool BgColorSpecialHandler::process (const char*, istream &, SpecialActions&) {
	return true;
}


static bool operator < (const pair<unsigned,Color> &pc1, const pair<unsigned,Color> &pc2) {
	// order PageColor objects by page number
	return pc1.first < pc2.first;
}


void BgColorSpecialHandler::dviBeginPage (unsigned pageno, SpecialActions &actions) {
	// Ensure that the background color of the preceeding page is set as the
	// default background color of the current page because this special affects
	// the current and all subsequent pages until the next change.
	// See the documentation of the color package, section 3.5.
	if (_pageColors.empty())
		return;
	// find number of page with bg color change not lower than the current one
	vector<PageColor>::iterator it = lower_bound(_pageColors.begin(), _pageColors.end(), PageColor(pageno, Color::BLACK));
	if (it != _pageColors.end() && it->first == pageno)
		actions.setBgColor(it->second);
	else if (it != _pageColors.begin())
		actions.setBgColor((--it)->second);
}


const char** BgColorSpecialHandler::prefixes () const {
	static const char *pfx[] = {"background", 0};
	return pfx;
}
