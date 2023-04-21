#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/tap.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/operators/map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_tap() {
  log() << "test_tap -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | tap<int>({
    [](auto&& x) { log() << "tap next " << x << std::endl; },
    [](auto err) { log() << "tap error" << std::endl; },
    []() { log() << "tap completed" << std::endl; }
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