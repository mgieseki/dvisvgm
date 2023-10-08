add_library(md5 md5.c)
target_include_directories(md5 PUBLIC .)
target_compile_options(md5 PRIVATE -Wall)
target_compile_features(md5 PUBLIC c_std_11)
