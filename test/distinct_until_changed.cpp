#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/distinct_until_changed.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/subjects/behavior.h>
#include "common.h"
#include <random>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_distinct_until_changed() {
  log() << "test_distinct_until_changed -- begin" << std::endl;

  subjects::behavior<int> sbj(100);
  doSubscribe(sbj.as_observable() | distinct_until_changed());
  sbj.as_subscriber().on_next(100);
  sbj.as_subscriber().on_next(1);
  sbj.as_subscriber().on_next(1);
  sbj.as_subscriber().on_next(2);
  sbj.as_subscriber().on_next(2);
  sbj.as_subscriber().on_next(3);
  sbj.as_subscriber().on_next(4);
  sbj.as_subscriber().on_completed();

  log() << "test_distinct_until_changed -- end" << std::endl << std::endl;
}