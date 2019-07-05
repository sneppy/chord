#pragma once

/**
 * Log macro
 */
#if BUILD_RELEASE
	#define LOG(type, format, ...)
#else
	#define LOG(type, format, ...) \
		fprintf(stdout, #type ": " format, ##__VA_ARGS__)
#endif

/**
 * Error macro
 */
#define ERROR(type, format, ...) \
	fprintf(stderr, #type ": " format, ##__VA_ARGS__)
