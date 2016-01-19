# This file is part of dvisvgm
# Copyright (C) 2005-2016 Martin Gieseking <martin.gieseking@uos.de>
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([dvisvgm],[1.14.2],[martin.gieseking@uos.de])
DATE="January 2016"
AC_CONFIG_SRCDIR(src)
AC_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE

AH_TEMPLATE([TARGET_SYSTEM], [The machine triplet of the target system])
AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$target"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_PROG_RANLIB
AC_LANG(C)

AC_CHECK_FUNCS([ftime gettimeofday])
AC_CHECK_HEADERS([sys/time.h sys/timeb.h])
AC_HEADER_TIOCGWINSZ

AC_CHECK_HEADER([kpathsea/kpathsea.h], , 
	[AC_MSG_ERROR([please install the kpathsea development package])])
AC_CHECK_LIB(kpathsea, kpse_find_file, , 
	[AC_MSG_ERROR([libkpathsea not found, please install the corresponding package first])])

AC_MSG_CHECKING([kpathsea version])
AC_RUN_IFELSE([AC_LANG_SOURCE([#include <stdio.h>
	#include <kpathsea/kpathsea.h>
	int main() {
		FILE *f;
		f = fopen("kpseversion", "w");
		if(!f) exit(1);
		fprintf(f, "%s\n", KPSEVERSION);
		fclose(f);
		exit(0);
	}])], 
	[kpseversion=`cat kpseversion|sed 's/kpathsea version //'`], [kpseversion=], [kpseversion=unknown])
AC_MSG_RESULT("$kpseversion")
rm -f kpseversion
if test -z "$kpseversion"; then
	AC_MSG_ERROR([Could not compile a simple kpathsea program -- check your installation])
fi


# Check how to link potracelib
HAVE_LIBPOTRACE=1
AC_CHECK_HEADER([potracelib.h], 
	[AC_CHECK_LIB(m, sqrt) # required to pass potrace check
	 AC_CHECK_LIB(potrace, potrace_trace, , HAVE_LIBPOTRACE=0)], HAVE_LIBPOTRACE=0)

if test "$HAVE_LIBPOTRACE" -eq 0; then
	AC_MSG_ERROR([potrace library not found])
fi


# Check how to link Ghostscript
HAVE_LIBGS=1
AC_CHECK_HEADER([ghostscript/iapi.h], 
	AC_CHECK_LIB(gs, gsapi_new_instance, , HAVE_LIBGS=0), HAVE_LIBGS=0)

if test "$HAVE_LIBGS" -eq 0; then
	AC_CHECK_LIB(dl, dlopen, ,
		[AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support is to be disabled])
		 AC_MSG_WARN(PostScript support disabled)])
else
	# query Ghostscript version
	AC_MSG_CHECKING([Ghostscript version])
	AC_RUN_IFELSE([AC_LANG_SOURCE([#include <stdio.h>
		#include <ghostscript/iapi.h>
		int main () {
			gsapi_revision_t r;
			if (gsapi_revision(&r, sizeof(gsapi_revision_t)) == 0) {
				FILE *f; 
				f = fopen("gsversion", "w");
				if (!f) exit(1);
				fprintf(f, "%ld\n", r.revision);
				fclose(f);
				exit(0);
			}
		}])],
		[gsversion=`cat gsversion`], [gsversion=], [gsversion=unknown])
	AC_MSG_RESULT("$gsversion")
	rm -f gsversion
	if test "$gsversion" -lt 831; then
		# current Ghostscript API was introduced in version 8.31
		# older versions are not supported
		LIBS=`echo $LIBS | sed "s/-lgs //" $LIBS`
		AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])
		AC_MSG_WARN([Ghostscript version < 8.31 found; PostScript support disabled])
	fi
fi

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for libraries.
PKG_CHECK_MODULES([FREETYPE], [freetype2])

AC_CHECK_LIB(z, gzopen)

# Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h unistd.h])

# Check for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Check for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset strcasecmp strtol])

# add options for selection of "optional" library locations
# currently these libraries are mandatory; the --with-foo options
# are used to specify the locations explicitely

AC_ARG_WITH(zlib,[  --with-zlib=DIR         location of the zlib library],
	[if test "$withval" != no; then
		if test "$withval" != yes; then
			ZLIB_DIR=$withval
		fi
	fi])

if test "$withval" != no; then
	if test -n "$ZLIB_DIR"; then
		ZLIB_CFLAGS="-I$ZLIB_DIR -I$ZLIB_DIR/include"
		ZLIB_LIBS="-L$ZLIB_DIR/lib"
	fi
fi

AC_SUBST([ZLIB_CFLAGS])
AC_SUBST([ZLIB_LIBS])

# Check if the kpathsea headers are C++ safe
AC_MSG_CHECKING([if the kpathsea headers are C++ safe])
AC_LANG_PUSH([C++])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                [[printf("%s\n", concat("one", "two"))]])],
               [AC_MSG_RESULT([yes])],
               [AC_MSG_RESULT([no])
                AC_DEFINE([KPSE_CXX_UNSAFE], 1,
                          [Define to 1 if the kpathsea headers are not C++ safe.])])
AC_LANG_POP([C++])

AX_CODE_COVERAGE
AS_IF([ test "$enable_code_coverage" = "yes" ], [
	# disable optimization
	changequote({,})
	CFLAGS=`echo $CFLAGS | sed 's/-O[1-9s]//g'`
	CXXFLAGS=`echo $CXXFLAGS | sed 's/-O[1-9s]//g'`
	changequote([,])
])

AC_SUBST(DATE)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_CONFIG_FILES([Makefile clipper/Makefile xxHash/Makefile src/Makefile tests/Makefile doc/dvisvgm.txt doc/Makefile])
AC_OUTPUT

