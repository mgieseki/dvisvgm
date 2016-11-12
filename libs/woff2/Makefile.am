noinst_LIBRARIES = libwoff2.a
libwoff2_a_SOURCES = \
	src/buffer.h \
	src/file.h \
	src/font.cc \
	src/font.h \
	src/glyph.cc \
	src/glyph.h \
	src/normalize.cc \
	src/normalize.h \
	src/port.h \
	src/round.h \
	src/store_bytes.h \
	src/table_tags.cc \
	src/table_tags.h \
	src/transform.cc \
	src/transform.h \
	src/variable_length.cc \
	src/variable_length.h \
	src/woff2_common.cc \
	src/woff2_common.h \
	src/woff2_dec.h \
	src/woff2_enc.cc \
	src/woff2_enc.h \
	src/woff2_out.cc \
	src/woff2_out.h

EXTRA_DIST = LICENSE

AM_CXXFLAGS = -Wall -I$(srcdir)/brotli/enc

SUBDIRS = brotli

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
