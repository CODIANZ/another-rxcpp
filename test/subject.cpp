#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/subjects/subject.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_subject() {
  log() << "test_subject -- begin" << std::endl;

  thread_group threads;
  subjects::subject<int>  sbj;

  threads.push([sbj]() mutable {
    int i = 0;
    while(sbj.as_subscriber().is_subscribed()){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      sbj.as_subscriber().on_next(i++);
    }
    log() << "emit done" << std::endl;
  });

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

  subjects::subject<int> sbj2;
  sbj2.as_subscriber().on_error(std::make_exception_ptr(std::exception()));
  doSubscribe(sbj2.as_observable());

  threads.join_all();

  log() << "test_subject -- end" << std::endl << std::endl;
}