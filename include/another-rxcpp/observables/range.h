#if !defined(__another_rxcpp_h_range__)
#define __another_rxcpp_h_range__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  auto range(T start, T end) noexcept
    -> observable<typename internal::strip_const_reference<T>::type>
{
  using TT = typename internal::strip_const_reference<T>::type;
  return observable<>::create<TT>([start, end](subscriber<TT> s){
    for(TT i = start; i <= end; i++) {
      if(!s.is_subscribed()) break;
      s.on_next(i);
    }
    s.on_completed();
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_range__) */