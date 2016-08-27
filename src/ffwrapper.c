/*************************************************************************
** ffwrapper.c                                                          **
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

#include <fontforge.h>
#include "ffwrapper.h"

#ifdef _NDEBUG
static void no_post_error (const char *title, const char *error, ...) {}
static void no_warning (const char *format, ...) {}
#endif


void ff_init () {
	InitSimpleStuff();
#ifdef _NDEBUG
	ui_interface.post_error = &no_post_error;
	ui_interface.logwarning = &no_warning;
#endif
}


int ff_version () {
	return FONTFORGE_VERSIONDATE_RAW;
}


int ff_sfd_to_ttf (const char *sfdname, const char *ttfname, int autohint) {
	int ret=0;
	extern int autohint_before_generate;
	autohint_before_generate = autohint;
	SplineFont *sf = SFDRead((char*)sfdname);
	if (sf) {
		ret = WriteTTFFont((char*)ttfname, sf, ff_ttf, 0, 0, 0, sf->map, ly_fore);
		SplineFontFree(sf);
	}
	return ret;
}


int ff_sfd_to_woff (const char *sfdname, const char *woffname, int autohint) {
	int ret=0;
	extern int autohint_before_generate;
	autohint_before_generate = autohint;
	SplineFont *sf = SFDRead((char*)sfdname);
	if (sf) {
		ret = WriteWOFFFont((char*)woffname, sf, ff_woff, 0, 0, 0, sf->map, ly_fore);
		SplineFontFree(sf);
	}
	return ret;
}
