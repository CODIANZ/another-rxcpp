#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/skip_while.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_skip_while() {
  log() << "test_skip_while -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | skip_while([](auto x){
    return x < 90;
  }));

  log() << "test_skip_while -- end" << std::endl << std::endl;
}