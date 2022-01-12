#include <iostream>
#include <thread>

#include "common.h"

#define DO(f) { extern void f(); f(); }

int main() {
  log() << "**** start ****" << std::endl;

  DO(test_observable)
  DO(test_just)
  DO(test_range)
  DO(test_take)
  DO(test_flat_map)
  DO(test_map)
  DO(test_connectable)
  DO(test_subject)
  DO(test_error)
  DO(test_never)
  DO(test_empty)
  DO(test_on_error_resume_next)
  DO(test_retry)
  DO(test_subscribe_on)
  DO(test_new_thread_scheduler)
  DO(test_take_until)
  DO(test_merge)
  DO(test_amb)
  DO(test_behavior_subject)
  DO(test_distinct_until_changed)
  DO(test_interval)
  DO(test_delay)
  DO(test_tap)
  DO(test_async_scheduler)
  DO(test_finally)
  DO(test_take_last)
  DO(test_last)
  DO(test_timeout)
  DO(test_skip_while)
  DO(test_iterate)
  DO(test_blocking)
  DO(test_case_1)
  DO(test_ready_set_go)
  DO(test_inflow_restriction)

  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
    DO(test_operators_in_observable)
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    DO(test_rxcpp_compatible)
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

  log() << "**** finish ****" << std::endl;

  std::cout << "wait for some worker threads destruct objects (eg: sources in subscription)" << std::endl;
  wait(100);
}

