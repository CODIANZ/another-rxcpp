#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_take() {
  log() << "test_take -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | take(0));
  doSubscribe(o | take(1));
  doSubscribe(o | take(5));

  auto x = doSubscribe(
    o
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | take(10)
  );
  while(x.is_subscribed()) {}

  log() << "test_take -- end" << std::endl << std::endl;
}