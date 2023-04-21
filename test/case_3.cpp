#include <another-rxcpp/observable.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_case_3() {
  log() << "test_case_3 -- begin" << std::endl;

  auto sp = std::make_shared<int>(0);
  auto o = observable<>::create<int>([sp](subscriber<int> s){
    s.on_next(*sp);
    s.on_completed();
    *sp = *sp + 1;
  });

  doSubscribe(o);
  doSubscribe(o);

  log() << "test_case_3 -- end" << std::endl << std::endl;
}