#include <another-rxcpp/utils/ready_set_go.h>

#include <another-rxcpp/observable.h>
#include <another-rxcpp/subjects/subject.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_ready_set_go() {
  log() << "test_ready_set_go -- begin" << std::endl;

  subjects::subject<int> sbj;
  auto s = sbj.as_subscriber();
  auto o = utils::ready_set_go([=](){
    s.on_next(1);
  }, sbj.as_observable());

  auto x = doSubscribe(o);

  s.on_next(2);
  s.on_next(3);
  s.on_completed();

  log() << "test_ready_set_go -- end" << std::endl << std::endl;
}