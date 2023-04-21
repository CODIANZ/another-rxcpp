#if !defined(__another_rxcpp_h_just__)
#define __another_rxcpp_h_just__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers/default_scheduler.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto just(T&& value, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
    -> observable<typename internal::strip_const_reference<T>::type>
{
  using TT = typename internal::strip_const_reference<T>::type;
  auto v = std::make_shared<TT>(std::forward<T>(value));
  return observable<>::create<TT>([v, sccr](subscriber<TT> s) {
    auto scdl = sccr();
    scdl.schedule([v, s, scdl]() {
      s.on_next(*v);
      s.on_completed();
      scdl.abort();
    });
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_just__) */