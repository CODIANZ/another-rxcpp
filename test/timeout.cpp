#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/timeout.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/tap.h>
#include <another-rxcpp/operators/delay.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_timeout() {
  log() << "test_timeout -- begin" << std::endl;

  thread_group threads;
  auto o = observable<>::create<int>([threads](subscriber<int> s){
    threads.push([s]{
      for(int i = 1; i <= 10; i++){
        if(!s.is_subscribed()){
          log() << "interval_range break" << std::endl;
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(i * 100));
        if(!s.is_subscribed()){
          log() << "interval_range break" << std::endl;
          return;
        }
        log() << "interval_range emit " << i << std::endl;
        s.on_next(i);
      }
      log() << "interval_range complete" << std::endl;
      s.on_completed();
    });
  })
  | timeout(std::chrono::milliseconds(500));

  auto x = doSubscribe(o);
  
  while(x.is_subscribed()) {}
  threads.join_all();

  log() << "test_timeout -- end" << std::endl << std::endl;
}