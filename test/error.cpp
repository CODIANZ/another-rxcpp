#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_error() {
  log() << "test_error -- begin" << std::endl;

  auto o = observables::just(1)
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::error<int>(std::make_exception_ptr(std::exception()));
  });

  doSubscribe(o);
  
  log() << "test_error -- end" << std::endl << std::endl;
}