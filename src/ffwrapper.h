#ifndef FFWRAPPER_H
#define FFWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void ff_init ();
int ff_version ();
int ff_sfd_to_ttf (const char *sfdname, const char *ttfname, int autohint);
int ff_sfd_to_woff (const char *sfdname, const char *woffname, int autohint);

#ifdef __cplusplus
}
#endif

#endif

