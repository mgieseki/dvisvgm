# This file is part of dvisvgm
# Copyright (C) 2005-2020 Martin Gieseking <martin.gieseking@uos.de>
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([dvisvgm],[2.9.1],[martin.gieseking@uos.de])
DATE="March 2020"
AC_CONFIG_SRCDIR(src)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects])

AH_TEMPLATE([TARGET_SYSTEM], [The machine triplet of the target system])
AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$target"])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11])
AM_PROG_AR
LT_INIT

AC_PROG_RANLIB
AC_LANG(C)

AX_CHECK_COMPILE_FLAG([-Wmismatched-tags -Wno-mismatched-tags], [CXXFLAGS="$CXXFLAGS -Wno-mismatched-tags"])
AC_CHECK_HEADERS([sys/time.h sys/timeb.h xlocale.h])
AC_HEADER_TIOCGWINSZ

CPPFLAGS_SAVE="$CPPFLAGS"
CFLAGS_SAVE="$CFLAGS"
LDFLAGS_SAVE="$LDFLAGS"

# Check availability and usability of the kpathsea library
AC_ARG_VAR(KPSE_CFLAGS, [C/C++ compiler flags for the kpathsea library])
AC_ARG_VAR(KPSE_LIBS, [linker flags for the kpathsea library])
AC_ARG_VAR(KPSE_LIBS, [])
AC_ARG_WITH([kpathsea],
	[AS_HELP_STRING([--with-kpathsea=prefix], [set location of kpathsea library])],
	[with_kpathsea="$withval"],
	[with_kpathsea=yes])

AS_IF([test "x$with_kpathsea" != "xyes"],
	[KPSE_CFLAGS="-I$with_kpathsea/include" KPSE_LIBS="-L$with_kpathsea/lib"]
	[CPPFLAGS="$CPPFLAGS $KPSE_CFLAGS" CFLAGS="$CFLAGS $KPSE_CFLAGS" LDFLAGS="$LDFLAGS $KPSE_LIBS"])

AC_CHECK_HEADER([kpathsea/kpathsea.h],,
	[AC_MSG_ERROR([please install the kpathsea development package])])
AC_CHECK_LIB([kpathsea], [kpse_find_file],,
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
AS_IF([test -z "$kpseversion"],
	[AC_MSG_ERROR([Could not compile a simple kpathsea program -- check your installation])])
AC_SUBST(KPSE_CFLAGS)
AC_SUBST(KPSE_LIBS)

# Check how to link Ghostscript
have_libgs=yes
AC_CHECK_HEADER([ghostscript/iapi.h],
	[AC_CHECK_LIB(gs, gsapi_revision,, [have_libgs=no])],
	[have_libgs=no])
AS_IF([test "x$have_libgs" = "xno"],
	# Ghostscript not found, check for dlopen
	[AC_CHECK_LIB(dl, dlopen,,
		[AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])]
		[AC_MSG_WARN(PostScript support disabled)])])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for libraries.
PKG_CHECK_MODULES([FREETYPE], [freetype2])

AC_ARG_ENABLE([woff],
	[AS_HELP_STRING([--disable-woff], [Disable WOFF support @<:@default=no@:>@])],
	[],
	[enable_woff=yes])
AM_CONDITIONAL([ENABLE_WOFF], [test "x$enable_woff" = "xyes"])

AC_ARG_WITH([ttfautohint],
	[AS_HELP_STRING([--with-ttfautohint@<:@=prefix@:>@], [enable ttfautohint support (disabled by default)])],
	[with_ttfautohint="$withval"],
	[with_ttfautohint=no])

# Add option to enable linking of bundled libraries (brotli, potrace, woff2, xxhash).
AC_ARG_ENABLE([bundled-libs],
	[AS_HELP_STRING([--enable-bundled-libs], [use bundled libraries instead of the system ones @<:@default=no@:>@])])
AM_CONDITIONAL([USE_BUNDLED_LIBS], [test "x$enable_bundled_libs" = "xyes"])

# If option --enable-bundled-libs is not given, look for system libraries of brotli, potrace, woff2, and xxhash.
AS_IF([test "x$enable_bundled_libs" != "xyes"],
	[AC_CHECK_HEADER(potracelib.h,
		[AC_SEARCH_LIBS(potrace_trace, [potrace], [have_potrace=yes])])]
	[AC_CHECK_HEADER([xxhash.h],
		[AC_SEARCH_LIBS(XXH32, [xxhash], [have_xxhash=yes])])]
	[AS_IF([test "x$enable_woff" = "xyes"],
		[PKG_CHECK_MODULES(BROTLI, [libbrotlienc], [have_brotli=yes])]
		[PKG_CHECK_MODULES(WOFF2, [libwoff2enc], [have_woff2=yes])])])
AM_CONDITIONAL(HAVE_POTRACE, [test "x$have_potrace" = "xyes"])
AM_CONDITIONAL(HAVE_BROTLI, [test "x$have_brotli" = "xyes"])
AM_CONDITIONAL(HAVE_WOFF2, [test "x$have_woff2" = "xyes"])
AM_CONDITIONAL(HAVE_XXHASH, [test "x$have_xxhash" = "xyes"])

AS_IF([test "x$enable_woff" != "xyes"],
	[AC_DEFINE([DISABLE_WOFF], 1, [Define if WOFF support is disabled])],
	[AS_IF([test "x$with_ttfautohint" != "xno"],
		[AS_IF([test "x$with_ttfautohint" = "xyes"],
			# --with-ttfautohint without path => check via pkg-config
			[PKG_CHECK_MODULES([TTFAUTOHINT], [ttfautohint],,
				[AC_MSG_ERROR([can't locate ttfautohint, use "--with-ttfautohint=path" to specify its location])])],
			# --with-ttfautohint=/path/ttfautohint given
			[TTFAUTOHINT_CFLAGS="-I$with_ttfautohint/include"]
			[TTFAUTOHINT_LIBS="-L$with_ttfautohint/lib"])]
		[CPPFLAGS="$CPPFLAGS $TTFAUTOHINT_CFLAGS" CFLAGS="$CFLAGS $TTFAUTOHINT_CFLAGS" LDFLAGS="$LDFLAGS $TTFAUTOHINT_LIBS"]
		[AC_CHECK_HEADERS([ttfautohint.h],, [AC_MSG_WARN([ttfautohint.h not found])])]
		[AC_CHECK_LIB([ttfautohint], [TTF_autohint],,
			[AC_MSG_WARN([no working ttfautohint library found])]
			[AC_CHECK_LIB(dl, dlopen,
				[AC_MSG_NOTICE([enabled dynamic loading of ttfautohint])],
				[AC_MSG_WARN([disabled ttfautohint support])])])]
		[AC_SUBST(TTFAUTOHINT_CFLAGS) AC_SUBST(TTFAUTOHINT_LIBS)])])

AC_CHECK_HEADERS([openssl/md5.h])
PKG_CHECK_MODULES([LIBCRYPTO], [libcrypto], [HAVE_LIBCRYPTO=1], [HAVE_LIBCRYPTO=0])
AM_CONDITIONAL([USE_BUNDLED_MD5], [test "$HAVE_LIBCRYPTO" -eq 0])

AC_CHECK_LIB(z, gzopen)

# Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS_ONCE([libintl.h stdlib.h string.h strings.h unistd.h])

# Check for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Check for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS_ONCE([ftime gettimeofday sigaction umask uselocale])

# add options for selection of "optional" library locations
# currently these libraries are mandatory; the --with-foo options
# are used to specify the locations explicitely

AC_ARG_WITH(zlib,
	[AS_HELP_STRING([--with-zlib=DIR], [set location of the zlib library])],
	[AS_IF([test "x$withval" != "xno"],
		[AS_IF([test "x$withval" != "xyes"], [ZLIB_DIR=$withval])]
		[AS_IF([test -n "$ZLIB_DIR"],
			[ZLIB_CFLAGS="-I$ZLIB_DIR -I$ZLIB_DIR/include"]
			[ZLIB_LIBS="-L$ZLIB_DIR/lib" -lz])])])

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

CPPFLAGS="$CPPFLAGS_SAVE"
CFLAGS="$CFLAGS_SAVE"
LDFLAGS="$LDFLAGS_SAVE"

AC_ARG_ENABLE([manpage],
	[AS_HELP_STRING([--disable-manpage], [disable generation of manual page @<:@default=no@:>@])])
AS_IF([test "x$enable_manpage" != "xno"],[
	enable_manpage="no"
	# Check for utilities required to build the manpage
	AC_CHECK_PROG(ASCIIDOC, asciidoc, yes)
	AS_IF([test "x$ASCIIDOC" = "xyes"], [
		AC_CHECK_PROG(XMLTO, xmlto, yes)
		AS_IF([test "x$XMLTO" = "xyes"], [
			AC_CHECK_PROG(XSLTPROC, xsltproc, yes)
			AS_IF([test "x$XSLTPROC" = "xyes"],[enable_manpage="yes"])])])])
AS_IF([test "x$enable_manpage" != "xyes"],
	AC_MSG_NOTICE([generation of manual page has been disabled]))
AM_CONDITIONAL([BUILD_MANPAGE], [test "x$enable_manpage" = "xyes"])

AX_CODE_COVERAGE
AS_IF([ test "$enable_code_coverage" = "yes" ], [
	# disable optimization
	changequote({,})
	CFLAGS=`echo $CFLAGS | sed 's/-O[1-9s]//g'`
	CXXFLAGS=`echo $CXXFLAGS | sed 's/-O[1-9s]//g'`
	changequote([,])
])

AC_SUBST([dvisvgm_srcdir], ['$(top_srcdir)'])
AC_SUBST(DATE)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_CONFIG_FILES([
	Makefile
	libs/Makefile
	libs/brotli/Makefile
	libs/clipper/Makefile
	libs/ff-woff/Makefile
	libs/md5/Makefile
	libs/potrace/Makefile
	libs/variant/Makefile
	libs/woff2/Makefile
	libs/xxHash/Makefile
	m4/Makefile
	src/Makefile
	src/version.hpp
	src/optimizer/Makefile
	tests/Makefile
	tests/data/Makefile
	doc/Makefile])
AC_OUTPUT
