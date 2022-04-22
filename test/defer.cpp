#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_defer() {
  log() << "test_defer -- begin" << std::endl;

  auto obs_range = observables::defer([](){
    log() << "create range observable" << std::endl;
    return observables::range(1, 3);
  });

  log() << "range #1" << std::endl;
  doSubscribe(obs_range);

  log() << "range #2" << std::endl;
  doSubscribe(obs_range);

  auto obs_interval = observables::defer([](){
    log() << "create interval observable" << std::endl;
    return observables::interval(std::chrono::milliseconds(200), schedulers::new_thread_scheduler());
  });

  {
    log() << "interval #1" << std::endl;
    auto x = doSubscribe(obs_interval);
    wait(2000);
    x.unsubscribe();
    wait(2000);
  }

  {
    log() << "interval #2" << std::endl;
    auto x = doSubscribe(obs_interval);
    wait(2000);
    x.unsubscribe();
    wait(2000);
  }

  log() << "test_defer -- end" << std::endl << std::endl;
}