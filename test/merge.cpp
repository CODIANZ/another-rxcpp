#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/merge.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/subjects/subject.h>
#include <another-rxcpp/operators/map.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_merge() {

  log() << "test_merge -- begin" << std::endl;

  {
    log() << "completed" << std::endl;

    auto sbj1 = subjects::subject<int>();
    auto sbj2 = subjects::subject<int>();
    auto o = sbj1.as_observable()
    | merge(sbj2.as_observable());
    auto x = doSubscribe(o);

    sbj1.as_subscriber().on_next(11);
    sbj2.as_subscriber().on_next(21);
    sbj1.as_subscriber().on_next(12);
    sbj2.as_subscriber().on_next(22);
    sbj1.as_subscriber().on_completed();
    sbj2.as_subscriber().on_next(23);
    sbj2.as_subscriber().on_completed();
  }

  {
    log() << "error" << std::endl;

    auto sbj1 = subjects::subject<int>();
    auto sbj2 = subjects::subject<int>();
    auto o = sbj1.as_observable()
    | merge(sbj2.as_observable());
    auto x = doSubscribe(o);

    sbj1.as_subscriber().on_next(11);
    sbj2.as_subscriber().on_next(21);
    sbj1.as_subscriber().on_next(12);
    sbj2.as_subscriber().on_next(22);
    sbj1.as_subscriber().on_error(std::make_exception_ptr(std::exception()));
    sbj2.as_subscriber().on_next(23);
    sbj2.as_subscriber().on_completed();
  }

  {
    log() << "multi thread #1" << std::endl;
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
  }


  {
    log() << "multi thread #2" << std::endl;
    auto sbj1 = subjects::subject<int>();
    auto sbj2 = subjects::subject<int>();
    auto sbj3 = subjects::subject<int>();

    auto o = sbj1.as_observable() | merge(schedulers::new_thread_scheduler(), sbj2.as_observable(), sbj3.as_observable());

    auto x = doSubscribe(o);


    std::thread([sbj1](){
      for(int i = 100; i < 110; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        sbj1.as_subscriber().on_next(i);
      }
      sbj1.as_subscriber().on_completed();
    }).detach();

    std::thread([sbj2](){
      for(int i = 200; i < 210; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        sbj2.as_subscriber().on_next(i);
      }
      sbj2.as_subscriber().on_completed();
    }).detach();

    std::thread([sbj3](){
      for(int i = 300; i < 310; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(140));
        sbj3.as_subscriber().on_next(i);
      }
      sbj3.as_subscriber().on_completed();
    }).detach();

    while(x.is_subscribed()) {}
  }


  log() << "test_merge -- end" << std::endl << std::endl;
}