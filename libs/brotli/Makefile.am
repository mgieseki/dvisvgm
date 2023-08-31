if !HAVE_BROTLI
noinst_LIBRARIES = libbrotli.a
libbrotli_a_SOURCES = \
	common/constants.c \
	common/constants.h \
	common/context.c \
	common/context.h \
	common/dictionary.c \
	common/dictionary.h \
	common/platform.c \
	common/platform.h \
	common/shared_dictionary_internal.h \
	common/transform.c \
	common/transform.h \
	common/version.h \
	enc/backward_references.c \
	enc/backward_references.h \
	enc/backward_references_hq.c \
	enc/backward_references_hq.h \
	enc/backward_references_inc.h \
	enc/bit_cost.c \
	enc/bit_cost.h \
	enc/bit_cost_inc.h \
	enc/block_encoder_inc.h \
	enc/block_splitter.c \
	enc/block_splitter.h \
	enc/block_splitter_inc.h \
	enc/brotli_bit_stream.c \
	enc/brotli_bit_stream.h \
	enc/cluster.c \
	enc/cluster.h \
	enc/cluster_inc.h \
	enc/command.c \
	enc/command.h \
	enc/compound_dictionary.c \
	enc/compound_dictionary.h \
	enc/compress_fragment.c \
	enc/compress_fragment.h \
	enc/compress_fragment_two_pass.c \
	enc/compress_fragment_two_pass.h \
	enc/dictionary_hash.c \
	enc/dictionary_hash.h \
	enc/encode.c \
	enc/encoder_dict.c \
	enc/encoder_dict.h \
	enc/entropy_encode.c \
	enc/entropy_encode.h \
	enc/entropy_encode_static.h \
	enc/fast_log.c \
	enc/fast_log.h \
	enc/find_match_length.h \
	enc/hash_composite_inc.h \
	enc/hash_forgetful_chain_inc.h \
	enc/hash.h \
	enc/hash_longest_match64_inc.h \
	enc/hash_longest_match_inc.h \
	enc/hash_longest_match_quickly_inc.h \
	enc/hash_rolling_inc.h \
	enc/hash_to_binary_tree_inc.h \
	enc/histogram.c \
	enc/histogram.h \
	enc/histogram_inc.h \
	enc/literal_cost.c \
	enc/literal_cost.h \
	enc/memory.c \
	enc/memory.h \
	enc/metablock.c \
	enc/metablock.h \
	enc/metablock_inc.h \
	enc/params.h \
	enc/prefix.h \
	enc/quality.h \
	enc/ringbuffer.h \
	enc/state.h \
	enc/static_dict.c \
	enc/static_dict.h \
	enc/static_dict_lut.h \
	enc/utf8_util.c \
	enc/utf8_util.h \
	enc/write_bits.h \
	include/brotli/decode.h \
	include/brotli/encode.h \
	include/brotli/port.h \
	include/brotli/shared_dictionary.h \
	include/brotli/types.h

AM_CFLAGS = -Wall -I$(srcdir)/include

endif

EXTRA_DIST = LICENSE
CLEANFILES = *.gcda *.gcno
