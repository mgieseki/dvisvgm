#pragma once

#define WARNING(...) \
	ColoredPrintf(testing::internal::GTestColor::kYellow, "[ WARNING  ] "), \
	ColoredPrintf(testing::internal::GTestColor::kYellow, __VA_ARGS__), \
	printf("\n")

