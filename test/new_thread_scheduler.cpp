#include <another-rxcpp/schedulers/new_thread_scheduler.h>

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/observe_on.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::schedulers;

void test_new_thread_scheduler() {
  log() << "test_new_thread_scheduler -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | observe_on(new_thread_scheduler())
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::just(x);
  });

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  log() << "test_new_thread_scheduler -- end" << std::endl << std::endl;
}