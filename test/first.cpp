#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/first.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/operators/flat_map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_first() {
  log() << "test_first -- begin" << std::endl;

  doSubscribe(observables::range(1, 100) | first());

  auto x = doSubscribe(
    interval_range(1, 100, 100)
    | first()
  );
  while(x.is_subscribed()) {}

  log() << "test_first -- end" << std::endl << std::endl;
}