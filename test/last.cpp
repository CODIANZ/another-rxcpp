#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/last.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/operators/flat_map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_last() {
  log() << "test_last -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | last());

  auto x = doSubscribe(
    o
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | last()
  );
  while(x.is_subscribed()) {}

  log() << "test_last -- end" << std::endl << std::endl;
}