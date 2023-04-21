#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/timeout.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/tap.h>
#include <another-rxcpp/operators/delay.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_timeout() {
  log() << "test_timeout -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | tap<int>({
    [](const int& x){
    }
  })
  | flat_map([](int x){
    return observables::just(x)
    | delay(std::chrono::milliseconds(x * 100));
  })
  | tap<int>({
    [](const int& x){
    }
  })
  | timeout(std::chrono::milliseconds(500));

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  log() << "test_timeout -- end" << std::endl << std::endl;
}