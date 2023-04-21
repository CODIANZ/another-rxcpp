#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/finally.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::schedulers;

void test_finally() {
  log() << "test_finally -- begin" << std::endl;

  auto o = observables::just(1)
  | map([](int x){
    log() << x << std::endl;
    return x + 1;
  })
  | finally([](){
    log() << "finally #1" << std::endl;
  })
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::just(x);
  })
  | finally([](){
    log() << "finally #2" << std::endl;
  });

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  log() << "test_finally -- end" << std::endl << std::endl;
}