noinst_LIBRARIES = libbrotli.a
libbrotli_a_SOURCES = \
	backward_references.cc \
	backward_references.h \
	bit_cost.h \
	block_splitter.cc \
	block_splitter.h \
	brotli_bit_stream.cc \
	brotli_bit_stream.h \
	cluster.h \
	command.h \
	compress_fragment.cc \
	compress_fragment.h \
	compress_fragment_two_pass.cc \
	compress_fragment_two_pass.h \
	compressor.h \
	context.h \
	dictionary.cc \
	dictionary.h \
	dictionary_hash.h \
	encode.cc \
	encode.h \
	encode_parallel.cc \
	encode_parallel.h \
	entropy_encode.cc \
	entropy_encode.h \
	entropy_encode_static.h \
	fast_log.h \
	find_match_length.h \
	hash.h \
	histogram.cc \
	histogram.h \
	literal_cost.cc \
	literal_cost.h \
	metablock.cc \
	metablock.h \
	port.h \
	prefix.h \
	ringbuffer.h \
	static_dict.cc \
	static_dict.h \
	static_dict_lut.h \
	streams.cc \
	streams.h \
	transform.h \
	types.h \
	utf8_util.cc \
	utf8_util.h \
	write_bits.h

AM_CXXFLAGS = -Wall

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
