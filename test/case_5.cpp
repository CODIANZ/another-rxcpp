#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/interval.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/take.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include "common.h"
#include <thread>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;


void test_case_5() {
  log() << "test_case_5 -- begin" << std::endl;

  auto periodical = observables::interval(std::chrono::milliseconds(100), schedulers::default_scheduler())
    | tap([](auto x){
      log() << "tap " << x << std::endl;
    });

  /** check recursive unsubscribe in flat_map */

  auto sbsc = periodical
  | flat_map([periodical](int x){
    return periodical
    | flat_map([periodical](int x){
      return periodical;
    });
  })
  | take(1)
  | subscribe(
    [](auto x) {
      log() << "next: " << x << std::endl;
    },
    [](auto) {},
    []() {
      log() << "completed" << std::endl;
    }
  );

  while(sbsc.is_subscribed()) {}
  wait(1000);

  log() << "test_case_5 -- end" << std::endl << std::endl;
}