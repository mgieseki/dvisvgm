# This file is part of dvisvgm
# Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([dvisvgm],[0.8.9dev],[martin.gieseking@uos.de])
DATE="January 2010"
AC_CONFIG_SRCDIR(src)
AC_CONFIG_HEADER(config.h)
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

# Check whether MikTeX import libraries should be linked
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
		AC_TRY_RUN([#include <stdio.h>
  			#include <kpathsea/kpathsea.h>
			extern const char *kpathsea_version_string;
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


# Check how to link potracelib
HAVE_LIBPOTRACE=1
AC_CHECK_HEADER([potracelib.h], 
	[AC_CHECK_LIB(m, sqrt) # required to pass potrace check
	 AC_CHECK_LIB(potrace, potrace_trace, , HAVE_LIBPOTRACE=0)], HAVE_LIBPOTRACE=0)

if test "$HAVE_LIBPOTRACE" -eq 0; then
	AM_CPPFLAGS="$AM_CXXFLAGS -I../potracelib -I../../potracelib"
	EXTRA_LIBS="$EXTRA_LIBS ../potracelib/libpotrace.a "
	AC_MSG_NOTICE([Using bundled potrace library])
fi


# Check how to link Ghostscript
HAVE_LIBGS=1
AC_CHECK_HEADER([ghostscript/iapi.h], 
	AC_CHECK_LIB(gs, gsapi_new_instance, , HAVE_LIBGS=0), HAVE_LIBGS=0)

if test "$HAVE_LIBGS" -eq 0; then
	AC_CHECK_LIB(dl, dlopen, ,
		[AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])
		 AC_MSG_WARN(PostScript support disabled)])
else
	# query Ghostscript version
	AC_MSG_CHECKING([Ghostscript version])
	AC_TRY_RUN([#include <stdio.h>
		#include <ghostscript/iapi.h>
		int main () {
			gsapi_revision_t r;
			if (gsapi_revision(&r, sizeof(gsapi_revision_t)) == 0) {
				FILE *f; 
				f = fopen("gsversion", "w");
				if (!f) exit(1);
				fprintf(f, "%d\n", r.revision);
				fclose(f);
				exit(0);
			}
		}],
		[gsversion=`cat gsversion|cut -c1-3`], [gsversion=], [gsversion=unknown])
	AC_MSG_RESULT("$gsversion")
	rm -f gsversion >/dev/null
	if test "$gsversion" -lt 831; then
		# current Ghostscript API was introduced in version 8.31
		# older versions are not supported
		LIBS=`echo $LIBS | sed "s/-lgs //" $LIBS`
		AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])
		AC_MSG_WARN([Ghostscript version < 8.31; PostScript support disabled])
	fi
fi

# Check for libraries.
AC_CHECK_LIB(freetype, FT_Init_FreeType, , 
	[AC_MSG_ERROR([freetype library not found, please install it first])])

AC_CHECK_LIB(z, gzopen)
#AC_CHECK_FUNC(getopt_long, , EXTRA_OBJS="$EXTRA_OBJS getopt.o getopt1.o")

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

	if test -n "$ZLIB_DIR"; then
		CPPFLAGS="$CPPFLAGS -I$ZLIB_DIR -I$ZLIB_DIR/include"
		LDFLAGS="$LDFLAGS -L$ZLIB_DIR/lib"
	fi
fi


AC_SUBST(DATE)
AC_SUBST(EXTRA_LIBS)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_CONFIG_FILES([Makefile potracelib/Makefile src/Makefile tests/Makefile doc/dvisvgm.txt doc/Makefile])
AC_OUTPUT
