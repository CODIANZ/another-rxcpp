#if !defined(__h_never__)
#define __h_never__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  inline auto never()
    -> observable<T>
{
  return observable<>::create<T>([](subscriber<T> s){
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_never__) */