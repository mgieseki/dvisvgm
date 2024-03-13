#pragma once

#ifndef DVISVGM_CMAKE_BUILD
#define WARNING(...) \
	ColoredPrintf(testing::internal::GTestColor::kYellow, "[ WARNING  ] "), \
	ColoredPrintf(testing::internal::GTestColor::kYellow, __VA_ARGS__), \
	printf("\n")
#else
#define WARNING(...)
#endif
