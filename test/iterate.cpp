#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_iterate() {
  log() << "test_iterate -- begin" << std::endl;

  std::vector<int> arr = {1, 10, 1000, 10000, 100000};
  auto o = observables::iterate(arr);

  doSubscribe(o);

  log() << "test_iterate -- end" << std::endl << std::endl;
}