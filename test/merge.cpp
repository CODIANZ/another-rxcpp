#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_merge() {

  log() << "test_merge -- begin" << std::endl;

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

  auto emitter2 = emitter
  | map([](int x){
    return x + 1000;
  });

  auto emitter3 = emitter
  | map([](int x){
    return x + 2000;
  });

  auto o = emitter | merge(emitter2, emitter3);

  auto x = doSubscribe(o);

  while(x.is_subscribed()) {}

  log() << "test_merge -- end" << std::endl << std::endl;
}