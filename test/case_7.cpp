#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/subjects/subject.h>
#include <another-rxcpp/utils/ready_set_go.h>
#include "common.h"
#include <thread>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

struct watier : public std::enable_shared_from_this<watier> {
  subjects::subject<int> breaker_subject;

  virtual ~watier() {
    log() << "~watier" << std::endl;
  }

  auto wait_subscribe(std::string memo){
    auto THIS = shared_from_this();
    return observable<>::create<int>([THIS, memo](subscriber<int> s){
      observables::interval(std::chrono::milliseconds(700), schedulers::new_thread_scheduler())
      .take_until(THIS->breaker_subject.as_observable())
      .subscribe(
        [s, memo](auto x) {
          log() << memo << " next: " << x << std::endl;
        },
        [s, memo](auto err) {
          log() << memo << " error" << std::endl;
          s.on_error(err);
        },
        [s, memo]{
          log() << memo << " completed" << std::endl;
          s.on_next(999);
          s.on_completed();
        }
      );
    });
  }

  auto wait_stream_controller(std::string memo){
    auto THIS = shared_from_this();
    return observable<>::create<int>([THIS, memo](subscriber<int> s){
      internal::stream_controller<int> sctl(s);
      observables::interval(std::chrono::milliseconds(700), schedulers::new_thread_scheduler())
      .take_until(THIS->breaker_subject.as_observable())
      .subscribe(sctl.new_observer<int>(
        [sctl, memo](auto, auto x) {
          log() << memo << " next: " << x << std::endl;
        },
        [sctl, memo](auto, auto err) {
          log() << memo << " error" << std::endl;
          sctl.sink_error(err);
        },
        [sctl, memo](auto serial){
          log() << memo << " completed" << std::endl;
          sctl.sink_next(999);
          sctl.sink_completed(serial);
        }
      ));
    });
  }
};

auto emit_error(int ms){
  return observables::just(1)
  .observe_on(schedulers::new_thread_scheduler())
  .flat_map([ms](auto){
    return observable<>::create<int>([ms](subscriber<int> s){
      s.on_next(1);
      std::this_thread::sleep_for(std::chrono::milliseconds(ms));
      log() << "emit_error" << std::endl;
      s.on_error(std::make_exception_ptr(std::runtime_error("error")));
    });
  });
}

void test_case_7() {
  log() << "test_case_7 -- begin" << std::endl;

  {
    log() << "#1 -- begin" << std::endl;
    {
      auto w = std::make_shared<watier>();
      auto sbsc = emit_error(1000)
      .flat_map([w](auto){
        return w->wait_subscribe("#1 watier1")
          .amb(schedulers::new_thread_scheduler(), w->wait_subscribe("#1 watier2"));
      })
      .on_error_resume_next([w](auto){
        log() << "#1 on_error_resume_next" << std::endl;
        std::thread([w]{
          wait(100);
          log() << "#1 break!" << std::endl;
          w->breaker_subject.as_subscriber().on_next(1);
        }).detach();
        return observables::just(123456);
      })
      .subscribe(
        [](auto x) {
          log() << "#1 next: " << x << std::endl;
        },
        [](auto err) {
          log() << "#1 error" << std::endl;
        },
        []{
          log() << "#1 completed" << std::endl;
        }
      );
      while(sbsc.is_subscribed()) {}
    }
    log() << "#1-- end" << std::endl;
  }

  wait(1000);

  {
    log() << "#2 -- begin" << std::endl;
    {
      auto w = std::make_shared<watier>();
      auto sbsc = emit_error(1000)
      .flat_map([w](auto){
        return w->wait_stream_controller("#2 watier1")
          .amb(schedulers::new_thread_scheduler(), w->wait_stream_controller("#2 watier2"));
      })
      .on_error_resume_next([w](auto){
        log() << "#2 on_error_resume_next" << std::endl;
        std::thread([w]{
          wait(100);
          log() << "#2 break!" << std::endl;
          w->breaker_subject.as_subscriber().on_next(1);
        }).detach();
        return observables::just(123456);
      })
      .subscribe(
        [](auto x) {
          log() << "#2 next: " << x << std::endl;
        },
        [](auto err) {
          log() << "#2 error" << std::endl;
        },
        []{
          log() << "#2 completed" << std::endl;
        }
      );
      while(sbsc.is_subscribed()) {}
    }
    log() << "#2-- end" << std::endl;
  }

  wait(1000);

  log() << "test_case_7 -- end" << std::endl << std::endl;
}