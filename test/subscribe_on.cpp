#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/schedulers.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::schedulers;

void test_subscribe_on() {
  log() << "test_subscribe_on -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | subscribe_on(default_scheduler())
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::just(x);
  });

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  log() << "test_subscribe_on -- end" << std::endl << std::endl;
}