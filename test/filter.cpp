#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_filter() {
  log() << "test_filter -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | filter([](int x){ return x < 5; }));

  auto x = doSubscribe(
    o
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | filter([](int x){
      return x > 10 && x < 20;
    })
  );
  while(x.is_subscribed()) {}

  log() << "test_filter -- end" << std::endl << std::endl;
}