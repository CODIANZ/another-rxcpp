#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/utils.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::utils;

void test_something() {

  log() << "test_something -- begin" << std::endl;

  auto counter = std::make_shared<int>(0);

  auto o = observables::range(0, 10)
  | map([counter](int x){
    if(x == 3){
      (*counter)++;
      log() << "counter / value = " << *counter << " / " << x << std::endl;
      if(*counter < 3){
        log() << "retry" << std::endl;
        something<>::retry();
      }
      else{
        log() << "emit error" << std::endl;
        return something<>::error<int>(std::make_exception_ptr(std::exception()));
      }
    }
    log() << "counter / value = " << *counter << " / " << x << std::endl;
    return something<>::success(x);
  })
  | retry()
  | flat_map([](something<int> x){
    return x.proceed();
  });

  log() << "something()"  << std::endl;
  doSubscribe(o);
  
  log() << "test_something -- end" << std::endl << std::endl;
}