#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/publish.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_connectable() {
  log() << "test_connectable -- begin" << std::endl;

  thread_group threads;

  auto o = observable<>::create<int>([threads](subscriber<int> s){
    threads.push([s](){
      for(int i = 0; i < 100; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        log() << "emit " << i <<  std::endl;
        s.on_next(i);
      }
      log() << "break!" << std::endl;
    });
  });

  auto oo = o | publish();
  auto s1 = doSubscribe(oo);
  auto s2 = doSubscribe(oo);
  auto s3 = doSubscribe(oo);

  wait(1000);
  auto cc = oo.connect();

  wait(800);
  s1.unsubscribe();

  wait(800);
  s2.unsubscribe();

  auto s4 = doSubscribe(oo);

  wait(800);
  s3.unsubscribe();

  wait(3000);
  s4.unsubscribe();

  wait(800);
  cc.unsubscribe();
  threads.join_all();

  log() << "test_connectable -- end" << std::endl << std::endl;
}