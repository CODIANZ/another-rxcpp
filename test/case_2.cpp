#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include <another-rxcpp/utils.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

static observable<int> foo() {
  subjects::subject<int> sbj;
  auto s = sbj.as_subscriber();
  return utils::ready_set_go([s](){
    s.on_next(1);
    s.on_completed();
  }, sbj.as_observable());
}

void test_case_2() {
  log() << "test_case_2 -- begin" << std::endl;

  auto x = doSubscribe(foo());

  while(x.is_subscribed()) {}

  log() << "test_case_2 -- end" << std::endl << std::endl;
}