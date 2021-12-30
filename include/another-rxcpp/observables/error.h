#if !defined(__h_error__)
#define __h_error__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {

template <typename T> auto error(std::exception_ptr err) -> observable<T> {
  return observable<>::create<T>([err](subscriber<T> s){
    s.on_error(err);
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_error__) */