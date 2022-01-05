#if !defined(__h_just__)
#define __h_just__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename T>
  auto just(T&& value, scheduler::creator_fn sccr = schedulers::default_scheduler())
    -> observable<typename strip_const_referece<T>::type>
{
  return observable<>::just(value, sccr);
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_just__) */