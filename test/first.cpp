#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_first() {
  log() << "test_first -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | first());

  auto x = doSubscribe(
    o
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | first()
  );
  while(x.is_subscribed()) {}

  log() << "test_first -- end" << std::endl << std::endl;
}