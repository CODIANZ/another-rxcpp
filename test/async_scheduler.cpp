#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/schedulers.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::schedulers;

void test_async_scheduler() {
  log() << "test_async_scheduler -- begin" << std::endl;

  auto o = observables::just(1)
  | observe_on(async_scheduler())
  | map([](int x){
    log() << x << std::endl;
    return x + 1;
  })
  | observe_on(async_scheduler())
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::just(x);
  });

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  wait(1000);

  log() << "test_async_scheduler -- end" << std::endl << std::endl;
}