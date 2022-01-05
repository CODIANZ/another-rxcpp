#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/schedulers.h>
#include "common.h"

using namespace another_rxcpp;

void test_interval() {
  log() << "test_interval -- begin" << std::endl;

  auto x = doSubscribe(observables::interval(std::chrono::milliseconds(200), schedulers::new_thread_scheduler()));
  wait(2000);
  x.unsubscribe();
  wait(2000);

  log() << "test_interval -- end" << std::endl << std::endl;
}