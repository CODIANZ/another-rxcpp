#if !defined(__another_rxcpp_h_just__)
#define __another_rxcpp_h_just__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto just(T&& value, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
    -> observable<typename internal::strip_const_reference<T>::type>
{
  using TT = typename internal::strip_const_reference<T>::type;
  return observable<>::create<TT>([value = std::forward<T>(value), sccr](subscriber<TT> s) mutable /* for move */ {
    auto scdl = sccr();
    scdl.schedule([value = std::move(value), s]() mutable /* for move */ {
      s.on_next(std::move(value));
    });
    scdl.schedule([s](){
      s.on_completed();
    });
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_just__) */