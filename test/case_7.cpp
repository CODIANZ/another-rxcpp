#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/subjects/subject.h>
#include <another-rxcpp/utils/ready_set_go.h>
#include "common.h"
#include <thread>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_case_7() {
  log() << "test_case_7 -- begin" << std::endl;

  {
    log() << "#1 normal observer -- begin" << std::endl;
    auto sbsc = observable<>::create<int>([](subscriber<int> s){
      subjects::subject<int> sbj;
      observables::range(0, 10)
      .observe_on(schedulers::observe_on_new_thread())
      .take_until(sbj.as_observable())
      .subscribe(
        [s, sbj](auto x) {
          log() << "#1 inner next: " << x << std::endl;
          if(x == 5){
            sbj.as_subscriber().on_next(1);
            sbj.as_subscriber().on_completed();
          }
        },
        [s](auto err) {
          log() << "#1 inner error" << std::endl;
          s.on_error(err);
        },
        [s]{
          log() << "#1 inner completed" << std::endl;
          s.on_next(999);
          s.on_completed();
        }
      );
    })
    .subscribe(
      [](auto x)  { log() << "#1 outer next: " << x << std::endl; },
      [](auto)    { log() << "#1 outer error" << std::endl; },
      []          { log() << "#1 outer completed" << std::endl; }
    );
    while(sbsc.is_subscribed()) {}
    log() << "#1 normal observer -- end" << std::endl;
  }

  {
    log() << "#2 stream_controller -- begin" << std::endl;

    auto sbsc = observable<>::create<int>([](subscriber<int> s){
      internal::stream_controller<int> sctl(s);
      subjects::subject<int> sbj;
      observables::range(0, 10)
      .observe_on(schedulers::observe_on_new_thread())
      .take_until(sbj.as_observable())
      .subscribe(sctl.new_observer<int>(
        [sctl, sbj](auto, auto x) {
          log() << "#2 inner next: " << x << std::endl;
          if(x == 5){
            sbj.as_subscriber().on_next(1);
            sbj.as_subscriber().on_completed();
          }
        },
        [sctl](auto, auto err) {
          log() << "#2 inner error" << std::endl;
          sctl.sink_error(err);
        },
        [sctl](auto serial){
          log() << "#2 inner completed" << std::endl;
          sctl.sink_next(999);
          sctl.sink_completed(serial);
        }
      ));
    })
    .subscribe(
      [](auto x)  { log() << "#2 outer next: " << x << std::endl; },
      [](auto)    { log() << "#2 outer error" << std::endl; },
      []          { log() << "#2 outer completed" << std::endl; }
    );
    while(sbsc.is_subscribed()) {}

    log() << "#2 stream_controller -- end" << std::endl;
  }

  log() << "test_case_7 -- end" << std::endl << std::endl;
}