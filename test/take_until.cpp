#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_take_until() {

  log() << "test_take_until -- begin" << std::endl;

  subjects::subject<int> trigger;
  auto emitter = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i < 100; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });


  auto o = emitter | take_until(trigger.as_observable());

  auto x = doSubscribe(o);

  setTimeout([trigger](){
    trigger.as_subscriber().on_next(1);
    trigger.as_subscriber().on_completed();
  }, 2000);

  while(x.is_subscribed()) {}

  wait(1000);

  log() << "test_take_until -- end" << std::endl << std::endl;
}