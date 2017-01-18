#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <sstream>
#include "utility.hpp"

using namespace util;
using namespace std;


TEST(UtilityTest, trim) {
	ASSERT_EQ(trim(""), "");
	ASSERT_EQ(trim("   "), "");
	ASSERT_EQ(trim(" \n \t  "), "");
	ASSERT_EQ(trim("abcdefg"), "abcdefg");
	ASSERT_EQ(trim("  abcdefg"), "abcdefg");
	ASSERT_EQ(trim("abcdefg  "), "abcdefg");
	ASSERT_EQ(trim("   abcdefg  "), "abcdefg");
	ASSERT_EQ(trim("   abc   defg  "), "abc   defg");
	ASSERT_EQ(trim("   abc \n  defg  \n \r"), "abc \n  defg");
}


TEST(UtilityTest, normalize_space) {
	ASSERT_EQ(normalize_space(""), "");
	ASSERT_EQ(normalize_space("   "), "");
	ASSERT_EQ(normalize_space(" \n \t  "), "");
	ASSERT_EQ(normalize_space("abcdefg"), "abcdefg");
	ASSERT_EQ(normalize_space("  abcdefg"), "abcdefg");
	ASSERT_EQ(normalize_space("abcdefg  "), "abcdefg");
	ASSERT_EQ(normalize_space("   abcdefg  "), "abcdefg");
	ASSERT_EQ(normalize_space("   abc   defg  "), "abc defg");
	ASSERT_EQ(normalize_space("   abc \n  defg  \n \r"), "abc defg");
	ASSERT_EQ(normalize_space("   abc \n  de\r\ffg  \n \r"), "abc de fg");
}


TEST(UtilityTest, tolower) {
	string str;
	ASSERT_EQ(str, "");
	ASSERT_EQ(tolower(str), "");
	ASSERT_EQ(tolower(str="abcdefg"), "abcdefg");
	ASSERT_EQ(tolower(str="ABCDEFG"), "abcdefg");
	ASSERT_EQ(tolower(str="123XyZ456"), "123xyz456");
	ASSERT_EQ(tolower(str="123\nXyZ 456"), "123\nxyz 456");
	ASSERT_EQ(str, "123\nxyz 456");
}


TEST(UtilityTest, ilog10) {
	ASSERT_EQ(ilog10(-10), 0);
	ASSERT_EQ(ilog10(-1), 0);
	ASSERT_EQ(ilog10(0), 0);
	for (int i=1; i <= 1000; i++) {
		ASSERT_EQ(ilog10(i), static_cast<int>(log10(i))) << "i=" << i;
	}
	for (int i=1000; i < numeric_limits<int>::max()/10; i*=10) {
		ASSERT_EQ(ilog10(i-1), static_cast<int>(log10(i-1))) << "i=" << i;
		ASSERT_EQ(ilog10(i), static_cast<int>(log10(i))) << "i=" << i;
		ASSERT_EQ(ilog10(i+1), static_cast<int>(log10(i+1))) << "i=" << i;
	}
}


static string base64 (const string &str) {
	istringstream iss(str);
	ostringstream oss;
	base64_copy(istreambuf_iterator<char>(iss), istreambuf_iterator<char>(), ostreambuf_iterator<char>(oss));
	return oss.str();
}


TEST(UtilityTest, base64_copy) {
	ASSERT_EQ(base64(""), "");
	ASSERT_EQ(base64("a"), "YQ==");
	ASSERT_EQ(base64("ab"), "YWI=");
	ASSERT_EQ(base64("abc"), "YWJj");
	ASSERT_EQ(base64("abcd"), "YWJjZA==");
	ASSERT_EQ(base64("aBcDe\nFgHiJ\n"), "YUJjRGUKRmdIaUoK");
}
