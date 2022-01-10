#if !defined(__another_rxcpp_h_just__)
#define __another_rxcpp_h_just__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto just(T&& value, scheduler::creator_fn sccr = schedulers::default_scheduler())
    -> observable<typename strip_const_referece<T>::type>
{
  using TT = typename strip_const_referece<T>::type;
  auto _value = std::forward<T>(value);
  return observable<>::create<TT>([_value, sccr](subscriber<TT> s){
    auto scdl = sccr();
    scdl.schedule([_value, s](){
      s.on_next(std::move(_value));
    });
    scdl.schedule([s](){
      s.on_completed();
    });
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_just__) */