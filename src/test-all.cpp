/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "tests/CalculatorTest.h"

static CalculatorTest suite_CalculatorTest;

static CxxTest::List Tests_CalculatorTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CalculatorTest( "tests/CalculatorTest.h", 27, "CalculatorTest", suite_CalculatorTest, Tests_CalculatorTest );

static class TestDescription_CalculatorTest_test_eval : public CxxTest::RealTestDescription {
public:
 TestDescription_CalculatorTest_test_eval() : CxxTest::RealTestDescription( Tests_CalculatorTest, suiteDescription_CalculatorTest, 30, "test_eval" ) {}
 void runTest() { suite_CalculatorTest.test_eval(); }
} testDescription_CalculatorTest_test_eval;

static class TestDescription_CalculatorTest_test_variables : public CxxTest::RealTestDescription {
public:
 TestDescription_CalculatorTest_test_variables() : CxxTest::RealTestDescription( Tests_CalculatorTest, suiteDescription_CalculatorTest, 41, "test_variables" ) {}
 void runTest() { suite_CalculatorTest.test_variables(); }
} testDescription_CalculatorTest_test_variables;

static class TestDescription_CalculatorTest_test_exceptions : public CxxTest::RealTestDescription {
public:
 TestDescription_CalculatorTest_test_exceptions() : CxxTest::RealTestDescription( Tests_CalculatorTest, suiteDescription_CalculatorTest, 53, "test_exceptions" ) {}
 void runTest() { suite_CalculatorTest.test_exceptions(); }
} testDescription_CalculatorTest_test_exceptions;

#include "tests/FontManagerTest.h"

static FontManagerTest suite_FontManagerTest;

static CxxTest::List Tests_FontManagerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_FontManagerTest( "tests/FontManagerTest.h", 29, "FontManagerTest", suite_FontManagerTest, Tests_FontManagerTest );

static class TestDescription_FontManagerTest_test_fontID : public CxxTest::RealTestDescription {
public:
 TestDescription_FontManagerTest_test_fontID() : CxxTest::RealTestDescription( Tests_FontManagerTest, suiteDescription_FontManagerTest, 39, "test_fontID" ) {}
 void runTest() { suite_FontManagerTest.test_fontID(); }
} testDescription_FontManagerTest_test_fontID;

static class TestDescription_FontManagerTest_test_getFont : public CxxTest::RealTestDescription {
public:
 TestDescription_FontManagerTest_test_getFont() : CxxTest::RealTestDescription( Tests_FontManagerTest, suiteDescription_FontManagerTest, 46, "test_getFont" ) {}
 void runTest() { suite_FontManagerTest.test_getFont(); }
} testDescription_FontManagerTest_test_getFont;

#include "tests/StreamCounterTest.h"

static StreamCounterTest suite_StreamCounterTest;

static CxxTest::List Tests_StreamCounterTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_StreamCounterTest( "tests/StreamCounterTest.h", 28, "StreamCounterTest", suite_StreamCounterTest, Tests_StreamCounterTest );

static class TestDescription_StreamCounterTest_test_count : public CxxTest::RealTestDescription {
public:
 TestDescription_StreamCounterTest_test_count() : CxxTest::RealTestDescription( Tests_StreamCounterTest, suiteDescription_StreamCounterTest, 31, "test_count" ) {}
 void runTest() { suite_StreamCounterTest.test_count(); }
} testDescription_StreamCounterTest_test_count;

#include "tests/PageSizeTest.h"

static PageSizeTest suite_PageSizeTest;

static CxxTest::List Tests_PageSizeTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_PageSizeTest( "tests/PageSizeTest.h", 29, "PageSizeTest", suite_PageSizeTest, Tests_PageSizeTest );

static class TestDescription_PageSizeTest_test_resize : public CxxTest::RealTestDescription {
public:
 TestDescription_PageSizeTest_test_resize() : CxxTest::RealTestDescription( Tests_PageSizeTest, suiteDescription_PageSizeTest, 38, "test_resize" ) {}
 void runTest() { suite_PageSizeTest.test_resize(); }
} testDescription_PageSizeTest_test_resize;

static class TestDescription_PageSizeTest_test_exceptions : public CxxTest::RealTestDescription {
public:
 TestDescription_PageSizeTest_test_exceptions() : CxxTest::RealTestDescription( Tests_PageSizeTest, suiteDescription_PageSizeTest, 48, "test_exceptions" ) {}
 void runTest() { suite_PageSizeTest.test_exceptions(); }
} testDescription_PageSizeTest_test_exceptions;

#include <cxxtest/Root.cpp>
