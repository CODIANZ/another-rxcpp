#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
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