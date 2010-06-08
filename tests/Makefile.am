## This file is part of dvisvgm
## Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

AM_CXXFLAGS = -I../src -I../../src -Wall

TESTS =
check_PROGRAMS =

TESTS += BoundingBoxTest
check_PROGRAMS += BoundingBoxTest
BoundingBoxTest_SOURCES = BoundingBoxTest.cpp
BoundingBoxTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += CalculatorTest
check_PROGRAMS += CalculatorTest
CalculatorTest_SOURCES = CalculatorTest.cpp
CalculatorTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += ColorSpecialTest
check_PROGRAMS += ColorSpecialTest
ColorSpecialTest_SOURCES = ColorSpecialTest.cpp
ColorSpecialTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += CommandLineTest
check_PROGRAMS += CommandLineTest
CommandLineTest_SOURCES = CommandLineTest.cpp
CommandLineTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += DirectoryTest
check_PROGRAMS += DirectoryTest
DirectoryTest_SOURCES = DirectoryTest.cpp
DirectoryTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += FileFinderTest
check_PROGRAMS += FileFinderTest
FileFinderTest_SOURCES = FileFinderTest.cpp
FileFinderTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += FilePathTest
check_PROGRAMS += FilePathTest
FilePathTest_SOURCES = FilePathTest.cpp
FilePathTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += FontManagerTest
check_PROGRAMS += FontManagerTest
FontManagerTest_SOURCES = FontManagerTest.cpp
FontManagerTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += FontMapTest
check_PROGRAMS += FontMapTest
FontMapTest_SOURCES = FontMapTest.cpp
FontMapTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += GraphicPathTest
check_PROGRAMS += GraphicPathTest
GraphicPathTest_SOURCES = GraphicPathTest.cpp
GraphicPathTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += LengthTest
check_PROGRAMS += LengthTest
LengthTest_SOURCES = LengthTest.cpp
LengthTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += MatrixTest
check_PROGRAMS += MatrixTest
MatrixTest_SOURCES = MatrixTest.cpp
MatrixTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += PageRagesTest
check_PROGRAMS += PageRagesTest
PageRagesTest_SOURCES = PageRagesTest.cpp
PageRagesTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += PageSizeTest
check_PROGRAMS += PageSizeTest
PageSizeTest_SOURCES = PageSizeTest.cpp
PageSizeTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += SplittedCharInputBufferTest
check_PROGRAMS += SplittedCharInputBufferTest
SplittedCharInputBufferTest_SOURCES = SplittedCharInputBufferTest.cpp
SplittedCharInputBufferTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += StreamInputBufferTest
check_PROGRAMS += StreamInputBufferTest
StreamInputBufferTest_SOURCES = StreamInputBufferTest.cpp
StreamInputBufferTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTS += VectorStreamTest
check_PROGRAMS += VectorStreamTest
VectorStreamTest_SOURCES = VectorStreamTest.cpp
VectorStreamTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main

TESTDATA =
TESTDATA += frktest.dvi frktest-wf-cmp.svg frktest-nf-cmp.svg
TESTDATA += sample.dvi sample-wf-cmp.svg sample-nf-cmp.svg
EXTRA_DIST = check-conv normalize.xsl $(TESTDATA)
TESTS += check-conv
