## This file is part of dvisvgm
## Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>
##
## Process this file with automake.

AM_CXXFLAGS = -I$(srcdir)/../src -I$(srcdir)/../../src -Wall -DSRCDIR='"$(srcdir)"'

TESTS =
check_PROGRAMS =

TESTS += BoundingBoxTest
check_PROGRAMS += BoundingBoxTest
BoundingBoxTest_SOURCES = BoundingBoxTest.cpp
BoundingBoxTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += CRC32Test
check_PROGRAMS += CRC32Test
CRC32Test_SOURCES = CRC32Test.cpp
CRC32Test_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += CalculatorTest
check_PROGRAMS += CalculatorTest
CalculatorTest_SOURCES = CalculatorTest.cpp
CalculatorTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += ColorSpecialTest
check_PROGRAMS += ColorSpecialTest
ColorSpecialTest_SOURCES = ColorSpecialTest.cpp
ColorSpecialTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += CommandLineTest
check_PROGRAMS += CommandLineTest
CommandLineTest_SOURCES = CommandLineTest.cpp
CommandLineTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += DirectoryTest
check_PROGRAMS += DirectoryTest
DirectoryTest_SOURCES = DirectoryTest.cpp
DirectoryTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += FileFinderTest
check_PROGRAMS += FileFinderTest
FileFinderTest_SOURCES = FileFinderTest.cpp
FileFinderTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += FilePathTest
check_PROGRAMS += FilePathTest
FilePathTest_SOURCES = FilePathTest.cpp
FilePathTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += FileSystemTest
check_PROGRAMS += FileSystemTest
FileSystemTest_SOURCES = FileSystemTest.cpp
FileSystemTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += FontManagerTest
check_PROGRAMS += FontManagerTest
FontManagerTest_SOURCES = FontManagerTest.cpp
FontManagerTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += FontMapTest
check_PROGRAMS += FontMapTest
FontMapTest_SOURCES = FontMapTest.cpp
FontMapTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += GFReaderTest
check_PROGRAMS += GFReaderTest
GFReaderTest_SOURCES = GFReaderTest.cpp
GFReaderTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += GraphicPathTest
check_PROGRAMS += GraphicPathTest
GraphicPathTest_SOURCES = GraphicPathTest.cpp
GraphicPathTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += LengthTest
check_PROGRAMS += LengthTest
LengthTest_SOURCES = LengthTest.cpp
LengthTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += MatrixTest
check_PROGRAMS += MatrixTest
MatrixTest_SOURCES = MatrixTest.cpp
MatrixTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += MessageExceptionTest
check_PROGRAMS += MessageExceptionTest
MessageExceptionTest_SOURCES = MessageExceptionTest.cpp
MessageExceptionTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += PSInterpreterTest
check_PROGRAMS += PSInterpreterTest
PSInterpreterTest_SOURCES = PSInterpreterTest.cpp
PSInterpreterTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += PageRagesTest
check_PROGRAMS += PageRagesTest
PageRagesTest_SOURCES = PageRagesTest.cpp
PageRagesTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += PageSizeTest
check_PROGRAMS += PageSizeTest
PageSizeTest_SOURCES = PageSizeTest.cpp
PageSizeTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += PairTest
check_PROGRAMS += PairTest
PairTest_SOURCES = PairTest.cpp
PairTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += SplittedCharInputBufferTest
check_PROGRAMS += SplittedCharInputBufferTest
SplittedCharInputBufferTest_SOURCES = SplittedCharInputBufferTest.cpp
SplittedCharInputBufferTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += StreamInputBufferTest
check_PROGRAMS += StreamInputBufferTest
StreamInputBufferTest_SOURCES = StreamInputBufferTest.cpp
StreamInputBufferTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += StreamReaderTest
check_PROGRAMS += StreamReaderTest
StreamReaderTest_SOURCES = StreamReaderTest.cpp
StreamReaderTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += StreamWriterTest
check_PROGRAMS += StreamWriterTest
StreamWriterTest_SOURCES = StreamWriterTest.cpp
StreamWriterTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += TypesTest
check_PROGRAMS += TypesTest
TypesTest_SOURCES = TypesTest.cpp
TypesTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTS += VectorStreamTest
check_PROGRAMS += VectorStreamTest
VectorStreamTest_SOURCES = VectorStreamTest.cpp
VectorStreamTest_LDADD = ../src/libdvisvgm.a ../potracelib/libpotrace.a -lgtest -lgtest_main -lpthread

TESTDATA =
TESTDATA += frktest.dvi frktest-wf-cmp.svg frktest-nf-cmp.svg
TESTDATA += sample.dvi sample-wf-cmp.svg sample-nf-cmp.svg
EXTRA_DIST = check-conv normalize.xsl cmr10.600gf $(TESTDATA)
TESTS += check-conv
