#if !defined(__another_rxcpp_h_empty__)
#define __another_rxcpp_h_empty__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto empty() noexcept
    -> observable<T>
{
  return observable<>::create<T>([](subscriber<T> s){
    s.on_completed();
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_empty__) */