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

