#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_never() {
  log() << "test_never -- begin" << std::endl;

  auto o = observables::just(1)
  | flat_map([](int&& x){
    log() << x << std::endl;
    return observables::never<int>();
  });

  doSubscribe(o);
  
  log() << "test_never -- end" << std::endl << std::endl;
}