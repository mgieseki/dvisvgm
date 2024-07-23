pkg_check_modules(brotlienc libbrotlienc IMPORTED_TARGET)

if (NOT brotlienc_FOUND)
    set(SOURCES
        common/constants.c
        common/context.c
        common/dictionary.c
        common/platform.c
        common/transform.c
        enc/backward_references.c
        enc/backward_references_hq.c
        enc/bit_cost.c
        enc/block_splitter.c
        enc/brotli_bit_stream.c
        enc/cluster.c
        enc/command.c
        enc/compound_dictionary.c
        enc/compress_fragment.c
        enc/compress_fragment_two_pass.c
        enc/dictionary_hash.c
        enc/encode.c
        enc/encoder_dict.c
        enc/entropy_encode.c
        enc/fast_log.c
        enc/histogram.c
        enc/literal_cost.c
        enc/memory.c
        enc/metablock.c
        enc/static_dict.c
        enc/utf8_util.c
    )
    add_library(brotli ${SOURCES})
    target_include_directories(brotli PUBLIC include)
    target_compile_options(brotli PRIVATE -Wall)
    target_compile_features(brotli PUBLIC cxx_std_11)
else()
    add_library(brotli INTERFACE)
    target_link_libraries(brotli INTERFACE PkgConfig::brotlienc)
endif()
