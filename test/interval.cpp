#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/observables/interval.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include "common.h"

using namespace another_rxcpp;

void test_interval() {
  log() << "test_interval -- begin" << std::endl;

  auto x = doSubscribe(observables::interval(std::chrono::milliseconds(200), schedulers::new_thread_scheduler()));
  wait(2000);
  x.unsubscribe();
  wait(2000);

  log() << "test_interval -- end" << std::endl << std::endl;
}