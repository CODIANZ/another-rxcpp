#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/observables/range.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;

void test_range() {
  log() << "test_range -- begin" << std::endl;

  log() << "1 - 10" << std::endl;
  doSubscribe(observables::range(1, 10));

  log() << "-10 - 5" << std::endl;
  doSubscribe(observables::range(-10, 5));

  log() << "test_range -- end" << std::endl << std::endl;
}