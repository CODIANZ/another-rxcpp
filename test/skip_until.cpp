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

  subjects::subject<int> trigger;
  auto emitter = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i < 10; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });


  auto o = emitter | skip_until(trigger.as_observable());

  auto x = doSubscribe(o);

  setTimeout([trigger](){
    trigger.as_subscriber().on_next(1);
    trigger.as_subscriber().on_completed();
  }, 2000);

  while(x.is_subscribed()) {}

  wait(1000);

  log() << "test_skip_until -- end" << std::endl << std::endl;
}