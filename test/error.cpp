#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

struct my_err : public std::exception {
  std::string s = "my_err";
  virtual const char* what() const noexcept { return s.c_str(); }
};


void test_error() {
  log() << "test_error -- begin" << std::endl;

  {
    auto o = observables::just(1)
    | flat_map([](int x){
      log() << x << std::endl;
      return observables::error<int>(std::make_exception_ptr(my_err()));
    });
    doSubscribe(o);
  }

  {
    auto o = observables::just(1)
    | flat_map([](int x){
      log() << x << std::endl;
      return observables::error<int>(my_err());
    });
    doSubscribe(o);
  }


  log() << "test_error -- end" << std::endl << std::endl;
}