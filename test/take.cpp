#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/take.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/operators/flat_map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_take() {
  log() << "test_take -- begin" << std::endl;

  auto o = observables::range(1, 10);

  doSubscribe(o | take(0));
  doSubscribe(o | take(1));
  doSubscribe(o | take(5));

  thread_group threads;

  auto x = doSubscribe(
    interval_range(1, 10, 100, threads)
    | take(2)
  );
  while(x.is_subscribed()) {}
  threads.join_all();

  log() << "test_take -- end" << std::endl << std::endl;
}