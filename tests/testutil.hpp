#pragma once
#include "gtest/internal/gtest-port.h"

namespace testing::internal {
enum class GTestColor { kDefault, kRed, kGreen, kYellow };

GTEST_ATTRIBUTE_PRINTF_(2, 3)
void ColoredPrintf(GTestColor color, const char* fmt, ...);
}


#define WARNING(...) \
	ColoredPrintf(testing::internal::GTestColor::kYellow, "[ WARNING  ] "), \
	ColoredPrintf(testing::internal::GTestColor::kYellow, __VA_ARGS__), \
	printf("\n")

