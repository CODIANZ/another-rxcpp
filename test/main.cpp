#include <iostream>
#include <thread>

#include "common.h"

int main() {
  log() << "**** start ****" << std::endl;

  extern void test_observable();
  test_observable();

  extern void test_just();
  test_just();

  extern void test_range();
  test_range();

  extern void test_take();
  test_take();

  extern void test_flat_map();
  test_flat_map();

  extern void test_map();
  test_map();

  extern void test_connectable();
  test_connectable();

  extern void test_subject();
  test_subject();

  extern void test_error();
  test_error();

  extern void test_never();
  test_never();

  extern void test_on_error_resume_next();
  test_on_error_resume_next();

  extern void test_connectable();
  test_connectable();

  extern void test_retry();
  test_retry();

  extern void test_observe_on();
  test_observe_on();

  extern void test_subscribe_on();
  test_subscribe_on();

  extern void test_new_thread_scheduler();
  test_new_thread_scheduler();

  extern void test_take_until();
  test_take_until();

  extern void test_merge();
  test_merge();

  extern void test_amb();
  test_amb();

  extern void test_behavior_subject();
  test_behavior_subject();

  extern void test_distinct_until_changed();
  test_distinct_until_changed();

  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
    extern void test_operators_in_observable();
    test_operators_in_observable();
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    extern void test_rxcpp_compatible();
    test_rxcpp_compatible();
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

  log() << "**** finish ****" << std::endl;

  std::cout << "wait for some worker threads destruct objects (eg: sources in subscription)" << std::endl;
  wait(100);
}

