#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/observables/iterate.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"
#include <vector>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_iterate() {
  log() << "test_iterate -- begin" << std::endl;

  std::vector<int> arr = {1, 10, 1000, 10000, 100000};
  auto o = observables::iterate(arr);

  doSubscribe(o);

  log() << "test_iterate -- end" << std::endl << std::endl;
}