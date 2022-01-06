#if !defined(__h_error__)
#define __h_error__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {

template <typename T, typename ERR>
  auto error(ERR err)
    -> observable<T>
{
  return observable<>::error<T>(err);
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_error__) */