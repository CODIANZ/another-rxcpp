#if !defined(__h_interval__)
#define __h_interval__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename T = int>
  auto interval(std::chrono::milliseconds msec, scheduler scl = schedulers::default_scheduler())
    -> observable<T>
{
  return observable<>::create<T>([](subscriber<T> s){
    /** TODO: not implemented */
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_interval__) */
