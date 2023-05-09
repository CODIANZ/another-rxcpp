#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/delay.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators/observe_on.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_delay() {

  log() << "test_delay -- begin" << std::endl;

  thread_group threads;

  auto emitter = observable<>::create<int>([threads](subscriber<int> s){
    threads.push([s](){
      for(int i = 0; i <= 5; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        log() << "emit " << i << std::endl;
        s.on_next(i);
      }
      s.on_completed();
    });
  });


  auto o = emitter
    | observe_on(schedulers::new_thread_scheduler())
    | delay(std::chrono::milliseconds(500));

  auto x = doSubscribe(o);

  while(x.is_subscribed()) {}
  threads.join_all();

  log() << "test_delay -- end" << std::endl << std::endl;
}