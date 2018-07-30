add_library(potrace
	curve.c
	decompose.c
	potracelib.c
	trace.c
)
set_property(TARGET potrace PROPERTY C_STANDARD 99)
set(POTRACE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "potrace include directory")
target_include_directories(potrace PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${POTRACE_INCLUDE_DIR})
