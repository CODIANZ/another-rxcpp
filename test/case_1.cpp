#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_case_1() {
  log() << "test_take -- begin" << std::endl;

  subjects::subject<int> sbj;

  auto o = sbj.as_observable()
  | flat_map([](int x){
    log() << x << std::endl;
    if(x == 10) return observables::just(10);
    return observables::never<int>();
  })
  | take(1);

  auto x = doSubscribe(o);

  sbj.as_subscriber().on_next(1);
  sbj.as_subscriber().on_next(3);
  sbj.as_subscriber().on_next(10);
  sbj.as_subscriber().on_next(5);
  sbj.as_subscriber().on_next(6);

  while(x.is_subscribed()) {}

  log() << "test_take -- end" << std::endl << std::endl;
}