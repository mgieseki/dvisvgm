noinst_LIBRARIES = libclipper.a
libclipper_a_SOURCES = clipper.cpp clipper.hpp
EXTRA_DIST = License.txt

AM_CXXFLAGS = -Wall

clipper.cpp: clipper.hpp

@CODE_COVERAGE_RULES@

CLEANFILES = *.gcda *.gcno
