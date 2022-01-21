#if !defined(__another_rxcpp_h_something__)
#define __another_rxcpp_h_something__

#include "../observable.h"
#include "../internal/tools/util.h"
#include <exception>

namespace another_rxcpp {
namespace utils {

template<typename T = void> class something;

template <> class something<void> {
private:
  class retry_trigger : public std::exception {};
public:
  [[noreturn]] static void retry() {
    throw retry_trigger{};
  }
  template<typename T, typename TT = typename internal::strip_const_referece<T>::type>
  static something<TT> success(T v) {
    return something<TT>(
      observables::just(std::forward<T>(v))
    );
  }
  template<typename T> static something<T> error(std::exception_ptr err) {
    return something<T>(
      observables::error<T>(err)
    );
  }
};

template<typename T> class something {
friend class something<>;
private:
  observable<T> o_;
  something(observable<T> o) : o_(o) {}
public:
  observable<T> proceed() { return o_; }
};

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_something__) */