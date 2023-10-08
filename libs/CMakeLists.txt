include(FindPkgConfig)

pkg_check_modules(freetype2 REQUIRED freetype2 IMPORTED_TARGET)
add_library(freetype2 INTERFACE)
target_link_libraries(freetype2 INTERFACE PkgConfig::freetype2)

add_subdirectory(boost)
add_subdirectory(brotli)
add_subdirectory(clipper)
add_subdirectory(md5)
add_subdirectory(potrace)
add_subdirectory(variant)
add_subdirectory(woff2)
add_subdirectory(xxHash)
