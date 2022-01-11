#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_retry() {

  log() << "test_retry -- begin" << std::endl;

  auto counter = std::make_shared<int>(0);

  auto o = observables::range(0, 10)
  | flat_map([counter](int x){
    log() << "value = " << x << std::endl;
    if(x == 3){
      (*counter)++;
      log() << "counter = " << *counter << std::endl;
      if(*counter > 5){
        return observables::just(x);
      }
      else{
        log() << "retry" << std::endl;
        return observables::error<int>(std::make_exception_ptr(std::exception()));
      }
    }
    else return observables::just(x);
  });

  log() << "retry()"  << std::endl;
  doSubscribe(o | retry());

  *counter = 0;
  log() << "retry(1)"  << std::endl;
  doSubscribe(o | retry(1));
  
  log() << "test_retry -- end" << std::endl << std::endl;
}