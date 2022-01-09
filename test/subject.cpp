#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_subject() {
  log() << "test_subject -- begin" << std::endl;

  subjects::subject<int>  sbj;

  std::thread([sbj]() mutable {
    int i = 0;
    while(sbj.as_subscriber().is_subscribed()){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      sbj.as_subscriber().on_next(i++);
    }
    log() << "emit done" << std::endl;
  }).detach();

  wait(2500);
  auto s1 = doSubscribe(sbj.as_observable());

  wait(1000);
  auto s2 = doSubscribe(sbj.as_observable());

  wait(1000);
  auto s3 = doSubscribe(sbj.as_observable());

  wait(500);
  s1.unsubscribe();

  wait(700);
  s2.unsubscribe();

  wait(900);
  s3.unsubscribe();

  auto s4 = doSubscribe(sbj.as_observable());

  wait(2000);

  sbj.as_subscriber().on_completed();
  doSubscribe(sbj.as_observable());

  wait(1000);

  subjects::subject<int> sbj2;
  sbj2.as_subscriber().on_error(std::make_exception_ptr(std::exception()));
  doSubscribe(sbj2.as_observable());

  log() << "test_subject -- end" << std::endl << std::endl;
}