#if !defined(__h_scheduler__)
#define __h_scheduler__

#include "scheduler_interface.h"
#include <memory>

namespace another_rxcpp {

class scheduler {
public:
  using function_type = typename scheduler_interface::function_type;
private:
  using isp = std::shared_ptr<scheduler_interface>;
  isp interface_;

protected:

public:
  template <typename ISP> scheduler(ISP isp) :
    interface_(std::dynamic_pointer_cast<scheduler_interface>(isp)) {}

  void run(function_type f) const {
    interface_->run(f);
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_scheduler__) */