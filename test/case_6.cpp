#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/subjects/subject.h>
#include <another-rxcpp/utils/ready_set_go.h>
#include "common.h"
#include <thread>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;


void test_case_6() {
  log() << "test_case_6 -- begin" << std::endl;

  subjects::subject<int> sbj;
  auto vec = std::make_shared<std::vector<int>>(std::vector<int>{1, 2, 3, 4, 5, 6});

  auto getNext = [sbj, vec] {
    if(vec->size() == 0){
      sbj.as_subscriber().on_completed();
    } else {
      auto x = vec->front();
      vec->erase(vec->begin());
      sbj.as_subscriber().on_next(x);
    }
  };

  auto sbsc = utils::ready_set_go([getNext]{
    getNext();
  }, sbj.as_observable())
  .observe_on(schedulers::new_thread_scheduler())
  .flat_map([getNext](int x){
    if(x == 2){
      return observables::error<int>(std::make_exception_ptr(std::exception()))
      .on_error_resume_next([](auto e){
        return observables::just(-1);
      });
    }
    else return observables::just(x);
  })
  .map([getNext](auto x){
    log() << "map: " << x << std::endl;
    getNext();
    return x;
  })
  .last()
  .subscribe(
    [](auto x) {
      log() << "next: " << x << std::endl;
    },
    [](auto) {},
    []{
      log() << "completed" << std::endl;
    }
  );

  while(sbsc.is_subscribed()) {}

  log() << "test_case_6 -- end" << std::endl << std::endl;
}