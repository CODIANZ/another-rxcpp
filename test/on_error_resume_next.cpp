#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/on_error_resume_next.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/error.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_on_error_resume_next() {
  log() << "test_on_error_resume_next -- begin" << std::endl;

  auto o = ovalue(1)
  | flat_map([](int x){
    log() << x << std::endl;
    return observables::error<int>(std::make_exception_ptr(std::exception()));
  })
  | on_error_resume_next([](std::exception_ptr err){
    log() << "on_error_resume_next #1" << std::endl;
    return ovalue(2);
  })
  | map([](int x){
    log() << x << std::endl;
    throw std::exception();
    return x + 1;
  })
  | on_error_resume_next([](std::exception_ptr err){
    log() << "on_error_resume_next #2" << std::endl;
    throw std::exception();
    return ovalue(3);
  });

  doSubscribe(o);

  log() << "test_on_error_resume_next -- end" << std::endl << std::endl;
}