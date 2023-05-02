#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/skip_until.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/subjects/subject.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_skip_until() {

  log() << "test_skip_until -- begin" << std::endl;

  thread_group threads;
  subjects::subject<int> trigger;
  auto emitter = observable<>::create<int>([threads](subscriber<int> s){
    threads.push([s](){
      for(int i = 0; i < 10; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.on_next(i);
      }
      s.on_completed();
    });
  });


  auto o = emitter | skip_until(trigger.as_observable());

  auto x = doSubscribe(o);

  setTimeout([trigger](){
    trigger.as_subscriber().on_next(1);
    trigger.as_subscriber().on_completed();
  }, 2000);

  while(x.is_subscribed()) {}
  threads.join_all();

  log() << "test_skip_until -- end" << std::endl << std::endl;
}