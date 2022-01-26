#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_tap() {
  log() << "test_tap -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | tap<int>({
    .on_next = [](auto&& x) { log() << "tap next " << x << std::endl; },
    .on_error = [](auto err) { log() << "tap error" << std::endl; },
    .on_completed = []() { log() << "tap completed" << std::endl; }
  })
  | map([](int x){
    return x * 10;
  });
  doSubscribe(o);


  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
  {
    auto oo = observables::range(1, 10)
    .tap(
      [](auto x) { log() << "tap next " << x << std::endl; },
      [](auto err) { log() << "tap error" << std::endl; },
      []() { log() << "tap completed" << std::endl; }
    )
    .map([](auto x){
      return x * 10;
    });
    doSubscribe(oo);
  }
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  log() << "test_tap -- end" << std::endl << std::endl;
}