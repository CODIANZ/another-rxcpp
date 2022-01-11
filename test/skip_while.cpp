#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_skip_while() {
  log() << "test_skip_while -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | skip_while([](auto x){
    return x < 90;
  }));

  log() << "test_skip_while -- end" << std::endl << std::endl;
}