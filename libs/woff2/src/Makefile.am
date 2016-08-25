noinst_LIBRARIES = libwoff2.a
libwoff2_a_SOURCES = \
	buffer.h \
	file.h \
	font.cc \
	font.h \
	glyph.cc \
	glyph.h \
	normalize.cc \
	normalize.h \
	port.h \
	round.h \
	store_bytes.h \
	table_tags.cc \
	table_tags.h \
	transform.cc \
	transform.h \
	variable_length.cc \
	variable_length.h \
	woff2_common.cc \
	woff2_common.h \
	woff2_dec.h \
	woff2_enc.cc \
	woff2_enc.h \
	woff2_out.cc \
	woff2_out.h

AM_CXXFLAGS = -Wall -I$(srcdir)/../brotli/enc

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
