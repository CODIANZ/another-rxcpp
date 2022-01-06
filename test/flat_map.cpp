#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_flat_map() {
  log() << "test_flat_map -- begin" << std::endl;

  auto o = ovalue(1)
  | flat_map([](int x){
    log() << x << std::endl;
    return ovalue(x + 1);
  })
  | flat_map([](int x){
    log() << x << std::endl;
    return ovalue(std::string("abc"));
  });
  doSubscribe(o);

  log() << "test_flat_map -- end" << std::endl << std::endl;
}