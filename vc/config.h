/* Basic header file used to compile dvisvgm on Windows. */

/* Name of package */
#define PACKAGE "dvisvgm"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "martin.gieseking@uos.de"

/* Define to the full name of this package. */
#define PACKAGE_NAME "dvisvgm"

/* Version number of package */
#define VERSION "1.9.1"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "dvisvgm " VERSION

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "dvisvgm"

/* Define to the version of this package. */
#define PACKAGE_VERSION VERSION

#define HAVE_STRING_H 1

/* Prevent defining the macros min and max present in windows.h */
#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifdef __WIN64__
  #define TARGET_SYSTEM "x86_64-pc-win64"
#else
  #define TARGET_SYSTEM "i686-pc-win32"
#endif
