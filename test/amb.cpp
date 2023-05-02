#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/amb.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/operators/map.h>
#include "common.h"
#include <random>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_amb() {

  log() << "test_amb -- begin" << std::endl;

  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());      
  std::uniform_real_distribution<> rand(0.1, 0.5);
  thread_group threads;

  auto emitter = [threads](int sleep_time) {
    return observable<>::create<int>([sleep_time, threads](subscriber<int> s){
      threads.push([s, sleep_time](){
        for(int i = 0; i < 10; i++){
          if(!s.is_subscribed()) break;
          std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
          s.on_next(i);
        }
        s.on_completed();
      });
    });
  };

  auto emitter2 = emitter(rand(engine) * 100)
  | map([](int x){
    return x + 1000;
  });

  auto emitter3 = emitter(rand(engine) * 100)
  | map([](int x){
    return x + 2000;
  });

  auto o = emitter(rand(engine) * 100) | amb(emitter2, emitter3);

  auto x = doSubscribe(o);

  while(x.is_subscribed()) {}

  threads.join_all();

  log() << "test_amb -- end" << std::endl << std::endl;
}