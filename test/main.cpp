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

  extern void test_retry();
  test_retry();

  log() << "**** finish ****" << std::endl;

  std::cout << "wait for some worker threads destruct objects (eg: sources in subscription)" << std::endl;
  wait(100);
}

