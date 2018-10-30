if !HAVE_POTRACE
noinst_LIBRARIES = libpotrace.a
libpotrace_a_SOURCES = \
	auxiliary.h \
	bitmap.h \
	config.h \
	curve.c \
	curve.h \
	decompose.c \
	decompose.h \
	lists.h \
	potracelib.c \
	potracelib.h \
	progress.h \
	trace.c \
	trace.h

AM_CFLAGS = -Wall -I$(srcdir)

@CODE_COVERAGE_RULES@
endif

CLEANFILES = *.gcda *.gcno
