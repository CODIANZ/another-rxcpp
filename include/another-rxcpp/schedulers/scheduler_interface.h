#if !defined(__h_scheduler_interface__)
#define __h_scheduler_interface__

#include <functional>
#include <iostream>

namespace another_rxcpp {
namespace schedulers {

class scheduler_interface {
public:
  using function_type = std::function<void()>;
  scheduler_interface() = default;
  virtual ~scheduler_interface() = default;
  virtual void run(function_type) = 0;
};

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_scheduler_interface__) */