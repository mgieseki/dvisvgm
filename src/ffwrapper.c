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
