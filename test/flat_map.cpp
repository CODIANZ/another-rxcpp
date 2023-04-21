#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/flat_map.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
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