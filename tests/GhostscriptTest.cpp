#include <gtest/gtest.h>
#include "Ghostscript.hpp"


TEST(GhostscriptTest, available1) {
	const char *args[] = {"test", "-q", "-dNODISPLAY"};
	Ghostscript gs(3, args);
	ASSERT_TRUE(gs.available());
}


TEST(GhostscriptTest, available2) {
	Ghostscript gs;
	ASSERT_TRUE(gs.available());
}


TEST(GhostscriptTest, revision) {
	Ghostscript gs;
	ASSERT_FALSE(gs.revision(true).empty());
	ASSERT_FALSE(gs.revision(false).empty());
}


TEST(Ghostscript, error_name) {
	Ghostscript gs;
	ASSERT_EQ(gs.error_name(0), (const char*)0);
	ASSERT_STREQ(gs.error_name(-1), "unknownerror");
	ASSERT_STREQ(gs.error_name(1), "unknownerror");
	ASSERT_STREQ(gs.error_name(2), "dictfull");
}
