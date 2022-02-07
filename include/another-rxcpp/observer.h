#if !defined(__another_rxcpp_h_observer__)
#define __another_rxcpp_h_observer__

#include <functional>
#include <memory>

namespace another_rxcpp {

template <typename T = void> struct observer;

template <> struct observer<void> {
  template <typename T>
    using next_t    = std::function<void(const T&)>;
  using error_t     = std::function<void(std::exception_ptr)>;
  using completed_t = std::function<void()>;
};

template <typename T> struct observer {
  using value_type        = T;
  using sp                = std::shared_ptr<observer<value_type>>;
  using on_next_fn_t      = observer<>::next_t<value_type>;
  using on_error_fn_t     = observer<>::error_t;
  using on_completed_fn_t = observer<>::completed_t;
  on_next_fn_t        on_next;
  on_error_fn_t       on_error;
  on_completed_fn_t   on_completed;
  observer(
    on_next_fn_t n = {},
    on_error_fn_t e = {},
    on_completed_fn_t c = {}
  ) noexcept :
    on_next(n),
    on_error(e),
    on_completed(c) {}
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_observable__) */
