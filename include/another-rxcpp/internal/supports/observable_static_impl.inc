#include "../../observables.h"

namespace another_rxcpp {
  template <typename...ARGS> auto observable<void>::just(ARGS...args) {
    return observables::just(std::forward<ARGS>(args)...);
  }
  template <typename T, typename...ARGS> auto observable<void>::error(ARGS...args) {
    return observables::error<T>(std::forward<ARGS>(args)...);
  }
  template <typename T> auto observable<void>::never() {
    return observables::never<T>();
  }
  template <typename T> auto observable<void>::empty() {
    return observables::empty<T>();
  }
  template <typename T, typename...ARGS> auto observable<void>::range(T start, ARGS...args) {
    return observables::range<T>(std::forward<T>(start), std::forward<ARGS>(args)...);
  }
  template <typename T, typename...ARGS> auto observable<void>::interval(ARGS...args) {
    return observables::interval<T>(std::forward<ARGS>(args)...);
  }
  template <typename...ARGS> auto observable<void>::iterate(ARGS...args) {
    return observables::iterate(std::forward<ARGS>(args)...);
  }
}
