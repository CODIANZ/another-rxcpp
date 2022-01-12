#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_empty() {
  log() << "test_empty -- begin" << std::endl;

  auto o = observables::just(1)
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::empty<int>();
  });

  doSubscribe(o);
  
  log() << "test_empty -- end" << std::endl << std::endl;
}