#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)

#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_operators_in_observable() {
  log() << "test_operators_in_observable -- begin" << std::endl;

  auto o = ovalue(1)
  .flat_map([](const auto& x){
    log() << x << std::endl;
    return observables::error<int>(std::make_exception_ptr(std::exception()));
  })
  .on_error_resume_next([](auto err){
    log() << "operators_in_observable #1" << std::endl;
    return ovalue(2);
  })
  .map([](const auto& x){
    log() << x << std::endl;
    throw std::exception();
    return std::string("abc");
  })
  .on_error_resume_next([](auto err){
    log() << "operators_in_observable #2" << std::endl;
    throw std::exception();
    return ovalue(std::string("abc"));
  });

  doSubscribe(o);

  log() << "test_operators_in_observable -- end" << std::endl << std::endl;
}

#endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */