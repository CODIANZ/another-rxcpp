#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/error.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/observables/never.h>
#include <another-rxcpp/observables/empty.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/take.h>
#include <another-rxcpp/operators/retry.h>
#include <another-rxcpp/operators/take_until.h>
#include <another-rxcpp/subjects/subject.h>
#include "common.h"
#include <thread>
#include <atomic>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

struct emitters {
  std::shared_ptr<std::atomic<int>> run = std::make_shared<std::atomic<int>>(0);


  auto loop() {
    return observable<>::create<int>([run = run](auto s){
      std::thread([s, run]{
        (*run)++;
        for(int i = 0; s.is_subscribed(); i++){
          log() << "emit: " << i << std::endl;
          s.on_next(i);
          std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        (*run)--;
        log() << "!!!break!!!" << std::endl;
      }).detach();
    });
  }

  auto defer_error() {
    return observable<>::create<int>([run = run](auto s){
      std::thread([s, run]{
        s.on_next(0);
        (*run)++;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        log() << "!!!defer_error!!!" << std::endl;
        s.on_error(std::make_exception_ptr(std::exception()));
        (*run)--;
      }).detach();
    });
  }

};

void test_case_4() {
  log() << "test_case_4 -- begin" << std::endl;

  {
    emitters emitters;
    auto sbj = subjects::subject<int>();
    auto sbsc = emitters.loop()
    | take_until(sbj.as_observable())
    | flat_map([sbj](int x){
      if(x == 4) {
        log() << "subject next!!" << std::endl;
        sbj.as_subscriber().on_next(1);
      }
      return observables::empty<int>();
    })
    | subscribe(
      [](auto x){ log() << "next: " << x << std::endl; },
      [](auto e){ log() << "error" << std::endl; },
      [](){ log() << "complete" << std::endl; }
    );
    while(sbsc.is_subscribed()) {}
  }
  

  {
    emitters emitters;
    auto sbj = subjects::subject<int>();
    auto sbsc = emitters.defer_error()
    | flat_map([&](int){
      return emitters.loop();
    })
    | flat_map([&](int x){
      if(x == 5){
        log() << "!!!error!!!" << std::endl;
        return observables::error<int>(std::make_exception_ptr(std::exception()));
      }
      return emitters.loop(); 
    })
    | retry(1)
    | take_until(sbj.as_observable())
    | flat_map([sbj](int x){
      if(x == 4) {
        log() << "subject next!!" << std::endl;
        sbj.as_subscriber().on_next(1);
      }
      return observables::empty<int>();
    })
    | subscribe(
      [](auto x){ log() << "next: " << x << std::endl; },
      [](auto e){ log() << "error" << std::endl; },
      [](){ log() << "complete" << std::endl; }
    );

    while(sbsc.is_subscribed()) {}

    log() << "emitters.run = " << *emitters.run <<  std::endl;
  }

  wait(2000);

  log() << "test_case_4 -- end" << std::endl << std::endl;
}