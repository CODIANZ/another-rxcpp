#if !defined(__h_just__)
#define __h_just__

#include "../observable.h"
#include "../util.h"

namespace another_rxcpp {
namespace observables {

template <typename T> auto just(T&& value) -> observable<typename strip_const_referece<T>::type> {
  using TT = typename strip_const_referece<T>::type;
  auto _value = std::forward<T>(value);
  return observable<>::create<TT>([_value](subscriber<TT> s){
    s.on_next(std::move(_value));
    s.on_completed();
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_just__) */