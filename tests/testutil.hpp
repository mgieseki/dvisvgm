#pragma once

namespace testing {
	namespace internal {
		enum GTestColor {
			COLOR_DEFAULT,
			COLOR_RED,
			COLOR_GREEN,
			COLOR_YELLOW
		};

		void ColoredPrintf(GTestColor color, const char* fmt, ...);
	}
}

#define WARNING(...) \
	ColoredPrintf(testing::internal::COLOR_YELLOW, "[ WARNING  ] "), \
	ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__), \
	printf("\n")

