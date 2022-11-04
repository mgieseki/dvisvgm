if !HAVE_XXHASH
noinst_LIBRARIES = libxxhash.a

libxxhash_a_SOURCES = xxhash.c xxhash.h xxh3.h
AM_CXXFLAGS = -Wall

xxhash.c: xxhash.h xxh3.h

endif

CLEANFILES = *.gcda *.gcno

