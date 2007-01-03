## Copyright (C) 2001-2005 Peter Selinger.
## This file is part of potrace. It is free software and it is covered
## by the GNU General Public License. See the file COPYING for details.

## Process this file with automake to produce Makefile.in

#----------------------------------------------------------------------

noinst_LIBRARIES = libpotrace.a
libpotrace_a_SOURCES = curve.c curve.h trace.c trace.h decompose.c	\
 decompose.h potracelib.c potracelib.h lists.h auxiliary.h bitmap.h	\
 progress.h

#----------------------------------------------------------------------
# dynamic libraries:

#lib_LTLIBRARIES = libpotrace.la
#libpotrace_la_SOURCES = curve.c curve.h trace.c trace.h decompose.c	\
# decompose.h potracelib.c potracelib.h lists.h auxiliary.h bitmap.h
#libpotrace_la_LDFLAGS = -version-info 0:0:0

#----------------------------------------------------------------------
# other stuff to distribute


curve.o: potracelib.h lists.h curve.h auxiliary.h
decompose.o: potracelib.h curve.h auxiliary.h lists.h bitmap.h decompose.h
decompose.o: progress.h
potracelib.o: potracelib.h curve.h auxiliary.h decompose.h progress.h trace.h
potracelib.o: ../config.h
trace.o: potracelib.h curve.h auxiliary.h lists.h trace.h progress.h
