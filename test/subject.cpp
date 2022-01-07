#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_subject() {
  log() << "test_subject -- begin" << std::endl;

  auto sbj = std::make_shared<subjects::subject<int>>();

  std::weak_ptr<subjects::subject<int>> weak_sbj = sbj;
  std::thread([weak_sbj]() mutable {
    for(int i = 0; i < 100; i++){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto p = weak_sbj.lock();
      if(p) p->as_subscriber().on_next(i);
    }
    auto p = weak_sbj.lock();
    if(p) p->as_subscriber().on_completed();
  }).detach();

  wait(2500);
  auto s1 = doSubscribe(sbj->as_observable());

  wait(1000);
  auto s2 = doSubscribe(sbj->as_observable());

  wait(1000);
  auto s3 = doSubscribe(sbj->as_observable());

  wait(500);
  s1.unsubscribe();

  wait(500);
  s2.unsubscribe();

  wait(500);
  s3.unsubscribe();

  wait(1000);
  auto s4 = doSubscribe(sbj->as_observable());

  wait(2000);

  sbj = std::make_shared<subjects::subject<int>>();
  sbj->as_subscriber().on_error(std::make_exception_ptr(std::exception()));
  doSubscribe(sbj->as_observable());

  log() << "test_subject -- end" << std::endl << std::endl;
}