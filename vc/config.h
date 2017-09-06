/* Basic header file used to compile dvisvgm on Windows. */
#if !defined(DVISVGM_CONFIG_H) && defined(__cplusplus)
#define DVISVGM_CONFIG_H
#include "../src/version.hpp"

/* Name of package */
#define PACKAGE PROGRAM_NAME

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "martin.gieseking@uos.de"

/* Define to the full name of this package. */
#define PACKAGE_NAME PROGRAM_NAME

/* Version number of package */
#define VERSION PROGRAM_VERSION

/* Define to the full name and version of this package. */
#define PACKAGE_STRING PROGRAM_NAME+std::string(" ")+PROGRAM_VERSION

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME PROGRAM_NAME

/* Define to the version of this package. */
#define PACKAGE_VERSION VERSION

#define HAVE_STRING_H 1

#ifdef _WIN64
  #define TARGET_SYSTEM "x86_64-pc-win64"
#else
  #define TARGET_SYSTEM "i686-pc-win32"
#endif
#endif
