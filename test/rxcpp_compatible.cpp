#if defined(SUPPORTS_RXCPP_COMPATIBLE)

#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_rxcpp_compatible() {
  log() << "test_rxcpp_compatible -- begin" << std::endl;

  auto o = ovalue(1);

  auto test = observable<>::just(12)
  .flat_map([](auto){
    return observable<>::error<int>(std::exception());
  })
  .flat_map([](auto){
    return observable<>::never<int>();
  })
  .flat_map([](auto){
    return observable<>::range(1, 5);
  })
  .flat_map([](auto){
    return observable<>::interval(std::chrono::milliseconds(100));
  })
  .flat_map([](auto){
    auto arr = {1, 2, 3};
    return observable<>::iterate(arr);
  })
  .amb(o)
  .as_dynamic()
  .delay(std::chrono::seconds(1))
  .distinct_until_changed()
  .finally([](){})
  .flat_map([](auto x){ return observable<>::just(x); })
  .last()
  .map([](auto x){ return x + 1; })
  .merge(o)
  .observe_on(schedulers::observe_on_new_thread())
  .on_error_resume_next([](auto err){ return observable<>::error<int>(err); })
  .publish()
  .retry()
  .retry(123)
  .subscribe_on(schedulers::observe_on_new_thread())
  .skip_while([](auto x) { return true; })
  .take_last(1)
  .take_until(o)
  .take(100)
  .tap([](auto){})
  .timeout(std::chrono::hours(2))
  .as_blocking();

  log() << "test_rxcpp_compatible -- end" << std::endl << std::endl;
}

#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */