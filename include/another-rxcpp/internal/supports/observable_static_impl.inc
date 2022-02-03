#include "../../observables.h"

namespace another_rxcpp {
  template <typename...ARGS> auto observable<void>::just(ARGS&&...args) noexcept {
    return observables::just(std::forward<ARGS>(args)...);
  }
  template <typename T, typename...ARGS> auto observable<void>::error(ARGS&&...args) noexcept {
    return observables::error<T>(std::forward<ARGS>(args)...);
  }
  template <typename T> auto observable<void>::never() noexcept {
    return observables::never<T>();
  }
  template <typename T> auto observable<void>::empty() noexcept {
    return observables::empty<T>();
  }
  template <typename T, typename...ARGS> auto observable<void>::range(T&& start, ARGS&&...args) noexcept {
    return observables::range<T>(std::forward<T>(start), std::forward<ARGS>(args)...);
  }
  template <typename T, typename...ARGS> auto observable<void>::interval(ARGS&&...args) noexcept {
    return observables::interval<T>(std::forward<ARGS>(args)...);
  }
  template <typename...ARGS> auto observable<void>::iterate(ARGS&&...args) noexcept {
    return observables::iterate(std::forward<ARGS>(args)...);
  }
}
