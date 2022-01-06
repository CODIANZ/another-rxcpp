#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include <another-rxcpp/schedulers.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_delay() {

  log() << "test_delay -- begin" << std::endl;

  auto emitter = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i <= 5; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        log() << "emit " << i << std::endl;
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });


  auto o = emitter
    | observe_on(schedulers::new_thread_scheduler())
    | delay(std::chrono::milliseconds(500));

  auto x = doSubscribe(o);

  while(x.is_subscribed()) {}

  log() << "test_delay -- end" << std::endl << std::endl;
}