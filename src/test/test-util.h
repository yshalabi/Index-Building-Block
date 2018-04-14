
/*
 * test-util.h - This file contains declarations for test-util.cpp
 *
 * For those definitions that will not be used outside the testing framework
 * please do not include them into common.h. Put them here instead.
 */

#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

#include "common.h"

// This defines test printing. Note that this is not affected by the flag
// i.e. It always prints even under debug mode
#define test_printf(fmt, ...)                               \
    do {                                                    \
      fprintf(stderr, "%-24s: " fmt, __FUNCTION__, ##__VA_ARGS__); \
      fflush(stderr);                                       \
    } while(0);

// If called this function prints the current function name
// as the name of the test case - this always prints in any mode
#define PrintTestName() do { \
                          test_printf("=\n"); \
                          test_printf("========== %s ==========\n", \
                                      __FUNCTION__); \
                          test_printf("=\n"); \
                        } while(0);

// This macro adds a new test by declaring the test name and then 
// call the macro to print test name.
// Note that the test body definition must have 2 }}
#define AddNewTest(n) void n() { PrintTestName();
// Or use this macro to make it prettier
#define END_TEST }

#endif