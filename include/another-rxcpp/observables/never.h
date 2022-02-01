#if !defined(__another_rxcpp_h_never__)
#define __another_rxcpp_h_never__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto never() noexcept
    -> observable<T>
{
  return observable<>::create<T>([](subscriber<T> s){
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_never__) */