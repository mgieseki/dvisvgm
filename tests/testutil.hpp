#pragma once

#define WARNING(...) \
	ColoredPrintf(testing::internal::COLOR_YELLOW, "[ WARNING  ] "), \
	ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__), \
	printf("\n")

