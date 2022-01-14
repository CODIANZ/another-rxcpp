#if !defined(__another_rxcpp_h_observer__)
#define __another_rxcpp_h_observer__

#include <functional>
#include <memory>

namespace another_rxcpp {

template <typename T> struct observer {
  using value_type        = T;
  using sp                = std::shared_ptr<observer<value_type>>;
  using on_next_fn_t      = std::function<void(value_type)>;
  using on_error_fn_t     = std::function<void(std::exception_ptr)>;
  using on_completed_fn_t = std::function<void()>;
  on_next_fn_t        on_next = {};
  on_error_fn_t       on_error = {};
  on_completed_fn_t   on_completed = {};
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_observable__) */
