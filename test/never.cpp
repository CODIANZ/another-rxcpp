#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/observables/never.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/operators/flat_map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_never() {
  log() << "test_never -- begin" << std::endl;

  auto o = observables::just(1)
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::never<int>();
  });

  auto x = doSubscribe(o);
  x.unsubscribe();
  
  log() << "test_never -- end" << std::endl << std::endl;
}