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
  .flat_map([](const auto&){
    return observable<>::error<int>(std::exception());
  })
  .flat_map([](const auto&){
    return observable<>::never<int>();
  })
  .flat_map([](const auto&){
    return observable<>::range(1, 5);
  })
  .flat_map([](const auto&){
    return observable<>::empty<int>();
  })
  .flat_map([](const auto&){
    return observable<>::interval(std::chrono::milliseconds(100));
  })
  .flat_map([](const auto&){
    auto arr = {1, 2, 3};
    return observable<>::iterate(arr);
  })
  .amb(o)
  .as_dynamic()
  .delay(std::chrono::seconds(1))
  .distinct_until_changed()
  .finally([](){})
  .flat_map([](const auto& x){ return observable<>::just(x); })
  .last()
  .map([](const auto& x){ return x + 1; })
  .first()
  .filter([](const auto& x){
    return x < 1;
  })
  .merge(o)
  .observe_on(schedulers::observe_on_new_thread())
  .on_error_resume_next([](auto err){ return observable<>::error<int>(err); })
  .publish()
  .retry()
  .retry(123)
  .subscribe_on(schedulers::observe_on_new_thread())
  .skip_until(o)
  .skip_while([](const auto& x) { return true; })
  .take_last(1)
  .take_until(o)
  .take_while([](const auto& x) { return true; })
  .take(100)
  .tap([](const auto&){})
  .timeout(std::chrono::hours(2))
  .zip(o, o, o)
  .zip([](const auto& a, const auto& b, const auto& c, const auto& d){
    return std::get<0>(a);
  }, o, o, o)
  .as_blocking();

  log() << "test_rxcpp_compatible -- end" << std::endl << std::endl;
}

#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */