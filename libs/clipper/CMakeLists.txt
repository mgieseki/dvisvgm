add_library(clipper clipper.cpp)
target_include_directories(clipper PUBLIC .)
target_compile_options(clipper PRIVATE -Wall)
target_compile_features(clipper PUBLIC cxx_std_11)
