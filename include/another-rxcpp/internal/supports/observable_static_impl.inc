#include "../../observables.h"

namespace another_rxcpp {
  template <typename...ARGS> auto observable<void>::just(ARGS...args) {
    return observables::just(std::forward<ARGS...>(args...));
  }
  template <typename...ARGS> auto observable<void>::error(ARGS...args) {
    return observables::error(std::forward<ARGS...>(args...));
  }
  template <typename...ARGS> auto observable<void>::never(ARGS...args) {
    return observables::never(std::forward<ARGS...>(args...));
  }
  template <typename...ARGS> auto observable<void>::range(ARGS...args) {
    return observables::range(std::forward<ARGS...>(args...));
  }
  template <typename...ARGS> auto observable<void>::interval(ARGS...args) {
    return observables::interval(std::forward<ARGS...>(args...));
  }
}
