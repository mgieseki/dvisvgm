pkg_check_modules(woff2enc libwoff2enc IMPORTED_TARGET)

if (NOT woff2enc_FOUND)
    set(SOURCES
        src/font.cc
        src/glyph.cc
        src/normalize.cc
        src/table_tags.cc
        src/transform.cc
        src/variable_length.cc
        src/woff2_common.cc
        src/woff2_enc.cc
        src/woff2_out.cc
    )
    add_library(woff2 ${SOURCES})
    target_include_directories(woff2 PUBLIC include)
    target_compile_options(woff2 PRIVATE -Wall)
    target_link_libraries(woff2 PRIVATE brotli)
    target_compile_features(woff2 PUBLIC cxx_std_11)
else()
    add_library(woff2 INTERFACE)
    target_link_libraries(woff2 INTERFACE PkgConfig::woff2enc)
endif()
