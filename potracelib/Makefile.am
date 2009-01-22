VERSION=1.8
noinst_LIBRARIES = libpotrace.a
libpotrace_a_SOURCES = curve.c curve.h trace.c trace.h decompose.c	\
 decompose.h potracelib.c potracelib.h lists.h auxiliary.h bitmap.h	\
 progress.h

curve.o: potracelib.h lists.h curve.h auxiliary.h
decompose.o: potracelib.h curve.h auxiliary.h lists.h bitmap.h decompose.h
decompose.o: progress.h
potracelib.o: potracelib.h curve.h auxiliary.h decompose.h progress.h trace.h
potracelib.o: ../config.h
trace.o: potracelib.h curve.h auxiliary.h lists.h trace.h progress.h

