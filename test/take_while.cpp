#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_take_while() {
  log() << "test_take_while -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | take_while([](auto x){
    return x < 10;
  }));

  log() << "test_take_while -- end" << std::endl << std::endl;
}