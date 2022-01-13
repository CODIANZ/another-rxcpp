#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_connectable() {
  log() << "test_connectable -- begin" << std::endl;

  auto o = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i < 100; i++){
        if(!s.is_subscribed()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });

  auto oo = o | publish();
  auto s1 = doSubscribe(oo);
  auto s2 = doSubscribe(oo);
  auto s3 = doSubscribe(oo);

  wait(1000);
  oo.connect();

  wait(800);
  s1.unsubscribe();

  wait(800);
  s2.unsubscribe();

  auto s4 = doSubscribe(oo);

  wait(800);
  s3.unsubscribe();

  wait(3000);
  s4.unsubscribe();

  log() << "test_connectable -- end" << std::endl << std::endl;
}