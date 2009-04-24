# This file is part of dvisvgm
# Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([dvisvgm],[0.7.3],[martin.gieseking@uos.de])
DATE="April 2009"
AC_CONFIG_SRCDIR(src)
AC_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_PROG_RANLIB
AC_LANG(C)

# Check whether MikTeX import libraries should be linked
AC_CANONICAL_HOST
case $host in 
	*-mingw32* | *-windows*)
		CPPFLAGS="$CPPFLAGS -DMIKTEX"
		EXTRA_LIBS="miktex-core.a miktex-kps.a"
	;;
	*)
		AC_CHECK_HEADER([kpathsea/kpathsea.h], , 
			[AC_MSG_ERROR([please install the kpathsea development package])])
		AC_CHECK_LIB(kpathsea, kpse_find_file, , 
			[AC_MSG_ERROR([libkpathsea not found, please install the corresponding package first])])

		AC_MSG_CHECKING([kpathsea version])
#		AC_CHANGE_FLAGS([$KPATHSEA_CFLAGS], [$KPATHSEA_LDFLAGS], [-lkpathsea])
		AC_TRY_RUN([#include <stdio.h>
  			#include <kpathsea/kpathsea.h>
			extern char *kpathsea_version_string;
			int main() {
				FILE *f;
				f = fopen("conftestval", "w");
				if(!f) exit(1);
				fprintf(f, "%s\n", kpathsea_version_string);
				fclose(f);
    			exit(0);
			}], 
			[kpseversion=`cat conftestval`], [kpseversion=], [kpseversion=unknown])
#		AC_RESTORE_FLAGS
		AC_MSG_RESULT("$kpseversion")
		if test -z "$kpseversion"; then
	  		AC_MSG_ERROR([Could not compile a simple kpathsea program -- check your installation])
		fi
	;;
esac

# Check for libraries.
AC_CHECK_LIB(freetype, FT_Init_FreeType, , 
	[AC_MSG_ERROR([freetype library not found, please install it first])])

#AC_CHECK_LIB(m, sqrt) # required to pass potrace check
#AC_CHECK_LIB(potrace, potrace_trace)
AC_CHECK_LIB(z, gzopen)
AC_CHECK_FUNC(getopt_long, , EXTRA_OBJS="$EXTRA_OBJS getopt.o getopt1.o")

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
AC_ARG_WITH(freetype,[  --with-freetype=DIR     location of the FreeType2 library],
	[if test "$withval" != no; then
		if test "$withval" != yes; then
			FREETYPE_DIR=$withval
		fi
	 fi])
#AC_ARG_WITH(potrace,[  --with-potrace=DIR      location of the potrace library],
#	[if test "$withval" != no; then
#		if test "$withval" != yes; then
#			POTRACE_DIR=$withval
#		fi
#	fi])
AC_ARG_WITH(zlib,[  --with-zlib=DIR         location of the zlib library],
	[if test "$withval" != no; then
		if test "$withval" != yes; then
			ZLIB_DIR=$withval
		fi
	fi])


if test "$withval" != no; then
	if test -n "$FREETYPE_DIR"; then
		AC_PATH_PROG(FREETYPE_CONFIG, freetype-config,,[$FREETYPE_DIR/bin:$PATH])
	else
		AC_PATH_PROG(FREETYPE_CONFIG, freetype-config)
	fi
	
	if test -n "$FREETYPE_CONFIG"; then
		CPPFLAGS=`$CPPFLAGS $FREETYPE_CONFIG --cflags`
		if test -n "$FREETYPE_DIR"; then
			CPPFLAGS="$CPPFLAGS -I$FREETYPE_DIR/include"
		fi
	else 
		if test -n "$FREETYPE_DIR"; then
	   	CPPFLAGS="$CPPFLAGS -I$FREETYPE_DIR/include/freetype2 -I$FREETYPE_DIR/include"
			LDFLAGS="$LDFLAGS -L$FREETYPE_DIR/lib"
		fi
	fi

#	if test -n "$POTRACE_DIR"; then
#		CPPFLAGS="$CPPFLAGS -I$POTRACE_DIR -I$POTRACE_DIR/include"
#		LDFLAGS="$LDFLAGS -L$POTRACE_DIR/lib"
#	fi

	if test -n "$ZLIB_DIR"; then
		CPPFLAGS="$CPPFLAGS -I$ZLIB_DIR -I$ZLIB_DIR/include"
		LDFLAGS="$LDFLAGS -L$ZLIB_DIR/lib"
	fi
fi


AC_SUBST(DATE)
AC_SUBST(EXTRA_LIBS)
AC_CONFIG_FILES([Makefile potracelib/Makefile src/Makefile doc/dvisvgm.txt doc/Makefile])
AC_OUTPUT
