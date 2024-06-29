add_library(boost INTERFACE)
target_include_directories(boost INTERFACE .)
target_compile_features(boost INTERFACE cxx_std_11)
