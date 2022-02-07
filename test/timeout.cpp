#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/schedulers.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::schedulers;

void test_timeout() {
  log() << "test_timeout -- begin" << std::endl;

  auto o = observables::range(1, 10)
  | tap<int>({
    [](const int& x){
    }
  })
  | flat_map([](int x){
    return observables::just(x)
    | delay(std::chrono::milliseconds(x * 100));
  })
  | tap<int>({
    [](const int& x){
    }
  })
  | timeout(std::chrono::milliseconds(500));

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}

  log() << "test_timeout -- end" << std::endl << std::endl;
}