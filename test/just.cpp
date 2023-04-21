#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/observables/just.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_just() {
  log() << "test_just -- begin" << std::endl;

  doSubscribe(observables::just(1));
  doSubscribe(observables::just(std::string("abc")));
  doSubscribe(observables::just(1.23));
  
  log() << "test_just -- end" << std::endl << std::endl;
}