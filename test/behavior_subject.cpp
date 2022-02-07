#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/subjects.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_behavior_subject() {
  log() << "test_behavior_subject -- begin" << std::endl;

  {
    subjects::behavior<int> sbj(100);
    doSubscribe(sbj.as_observable());
    sbj.as_subscriber().on_next(1);
    doSubscribe(sbj.as_observable());
    sbj.as_subscriber().on_next(2);
    doSubscribe(sbj.as_observable());
    sbj.as_subscriber().on_completed();
    doSubscribe(sbj.as_observable());
  }

  {
    const std::string x = "abc";
    subjects::behavior<std::string> sbj(x);
    doSubscribe(sbj.as_observable());
    sbj.as_subscriber().on_completed();
  }
  
  log() << "test_behavior_subject -- end" << std::endl << std::endl;
}