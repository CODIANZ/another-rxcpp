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
  DO(test_observe_on)
  DO(test_subscribe_on)
  DO(test_new_thread_scheduler)
  DO(test_take_until)
  DO(test_take_while)
  DO(test_merge)
  DO(test_amb)
  DO(test_behavior_subject)
  DO(test_distinct_until_changed)
  DO(test_interval)
  DO(test_delay)
  DO(test_tap)
  DO(test_finally)
  DO(test_take_last)
  DO(test_last)
  DO(test_timeout)
  DO(test_skip_until)
  DO(test_skip_while)
  DO(test_iterate)
  DO(test_blocking)
  DO(test_ready_set_go)
  DO(test_inflow_restriction)
  DO(test_zip)
  DO(test_first)
  DO(test_filter)
  DO(test_something)
  DO(test_defer)

  DO(test_case_1)
  DO(test_case_2)
  DO(test_case_3)
  DO(test_case_4)
  DO(test_case_5)
  DO(test_case_6)
  DO(test_case_7)

  DO(move_check)

  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
    DO(test_operators_in_observable)
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    DO(test_rxcpp_compatible)
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

  log() << "**** finish ****" << std::endl;
}

