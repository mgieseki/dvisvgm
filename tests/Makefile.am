## This file is part of dvisvgm
## Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.


AM_CXXFLAGS = -I../src -I../../src -Wall

## rules and macros for the unit test environment
## CxxTest (http://cxxtest.sourceforge.net) must be installed if you want to build 
## and run the tests!
check_PROGRAMS = test-all

# "test-all" should automatically be executed when calling "make check"
TESTS = test-all

# the tests to be executed are implemented in the following header files
test_all_headers = CalculatorTest.h ColorSpecialTest.h CommandLineTest.h DirectoryTest.h \
	FontManagerTest.h FontMapTest.h FileFinderTest.h GraphicPathTest.h LengthTest.h MatrixTest.h \
   StreamCounterTest.h PageSizeTest.h SplittedCharInputBufferTest.h StreamInputBufferTest.h \
	VectorStreamTest.h

test_all_SOURCES      = $(test_all_headers) test-all.cpp 
test_all_LDADD        = ../src/libdvisvgm.a
test_all_DEPENDENCIES = ../src/libdvisvgm.a

# use CxxTest to create the complete testing application
test-all.cpp: $(test_all_headers)
	cxxtestgen --error-printer -o $@ $(test_all_headers)

CLEANFILES = test-all.cpp

