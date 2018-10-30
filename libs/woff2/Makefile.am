if !HAVE_WOFF2
noinst_LIBRARIES = libwoff2.a
libwoff2_a_SOURCES = \
	include/woff2/decode.h \
	include/woff2/encode.h \
	include/woff2/output.h \
	include/woff2/version.h \
	src/buffer.h \
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
	src/woff2_enc.cc \
	src/woff2_out.cc

AM_CXXFLAGS = -Wall -I$(srcdir)/include -I$(srcdir)/../brotli/include

@CODE_COVERAGE_RULES@
endif

EXTRA_DIST = LICENSE
CLEANFILES = *.gcda *.gcno
