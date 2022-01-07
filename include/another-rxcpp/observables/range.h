#if !defined(__h_range__)
#define __h_range__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  auto range(T start, T end)
    -> observable<typename strip_const_referece<T>::type>
{
  using TT = typename strip_const_referece<T>::type;
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

#endif /* !defined(__h_range__) */