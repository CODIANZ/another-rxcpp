#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;

void test_map() {
  log() << "test_map -- begin" << std::endl;

  auto o = ovalue(1)
  | map([](int&& x){
    log() << x << std::endl;
    return x + 1;
  })
  | map([](int&& x){
    log() << x << std::endl;
    return std::string("abc");
  });
  doSubscribe(o);

  log() << "test_map -- end" << std::endl << std::endl;
}