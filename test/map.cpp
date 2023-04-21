#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/map.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_map() {
  log() << "test_map -- begin" << std::endl;

  auto o = ovalue(1)
  | map([](int x){
    log() << x << std::endl;
    return x + 1;
  })
  | map([](int x){
    log() << x << std::endl;
    return std::string("abc");
  });
  doSubscribe(o);

  log() << "test_map -- end" << std::endl << std::endl;
}