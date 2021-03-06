
/*
 * test-util.h - This file contains declarations for test-util.cpp
 *
 * For those definitions that will not be used outside the testing framework
 * please do not include them into common.h. Put them here instead.
 */

#ifndef _TEST_UTIL_H
#define _TEST_UTIL_H

#include "common.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <iostream>

// This defines test printing. Note that this is not affected by the flag
// i.e. It always prints even under debug mode
#define test_printf(fmt, ...)                               \
    do {                                                    \
      fprintf(stderr, "%-24s: " fmt, __FUNCTION__, ##__VA_ARGS__); \
      fflush(stderr);                                       \
    } while(0);

/* 
 * class TestPrint - Wraps over std::cerr
 * 
 * We need this class because parameterized keys and values cannot be directly
 * printed using printf and format string. A template with overloaded methods
 * is needed. A space is also printed before the element
 */
class TestPrint {
 public:
  template <typename T>
  inline TestPrint &operator<<(const T &var) { std::cerr << " " << var; return *this; }
} test_out;

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
// Note that the block must be ended by 2 '}}'
#define BEGIN_TEST(n) void n() { PrintTestName();
// Or use this macro to make it prettier
#define END_TEST }

// This requires the test to run under debug mode
#define BEGIN_DEBUG_TEST(n) void n() { \
  PrintTestName(); \
  IF_NDEBUG(err_printf("The test must be run under debug mode\n"));

/*
 * StartThread() - Starts a given number of threads with parameters
 * 
 * The thread function must have the first argument being size_t, which is
 * the current thread ID, starting from 0
 */
template <typename Function, typename... Args>
void StartThread(size_t thread_num, 
                 Function &&fn, 
                 Args &&...args) {
  // Store thread ID here
  std::thread thread_group[thread_num];

  // Launch a group of threads
  for (size_t thread_id = 0; thread_id < thread_num; thread_id++) {
    thread_group[thread_id] = std::thread{fn, thread_id, std::ref(args...)};
  }

  // Join the threads with the main thread
  for (size_t thread_id = 0; thread_id < thread_num; ++thread_id) {
    thread_group[thread_id].join();
  }

  return;
}

/*
 * _TestAssertionFail() - This function forks a new process to test whether 
 *                        assertion would fail
 * 
 * Returns 0 if assertion not triggered; 1 if triggered
 */
template <typename Function>
bool _TestAssertionFail(Function &&fn) {
  pid_t fork_ret = fork();
  // Did not fork, only one process
  if(fork_ret == -1) {
    err_printf("Fork() returned -1; exit\n");
  } else if(fork_ret == 0) {
    fn();
    // If the call does not fail then we exit with status code 0
    // which can be detected by the parent process
    exit(0);
  } else {
    int child_status = 0;
    int exit_pid = waitpid(fork_ret, &child_status, 0);
    if(exit_pid == -1) {
      err_printf("waitpid() returned -1; exit\n");
    } else {
      const int exit_status = WEXITSTATUS(child_status);
      const int exited_flag = WIFEXITED(child_status);
      test_printf("Child process %d returns with status %d (exit flag %d)\n", 
                  exit_pid, exit_status, exited_flag);
      // If not zero then it failed
      return !exited_flag;
    }
  }
}

// This macro automates the declaration of lambda
#define TestAssertionFail(s) _TestAssertionFail([&](){ s; })

#endif