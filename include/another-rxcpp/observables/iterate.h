#if !defined(__another_rxcpp_h_iterate__)
#define __another_rxcpp_h_iterate__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  auto iterate(T arr) noexcept
    -> observable<typename T::value_type>
{
  using TT = typename T::value_type;
  return observable<>::create<TT>([arr](subscriber<TT> s){
    for(auto it = std::cbegin(arr); it != std::cend(arr); it++) {
      if(!s.is_subscribed()) break;
      s.on_next(*it);
    }
    s.on_completed();
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_iterate__) */