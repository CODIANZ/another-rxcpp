#if !defined(__h_default_scheduler__)
#define __h_default_scheduler__

#include "scheduler.h"

namespace another_rxcpp {
namespace schedulers {

class default_scheduler_interface : public scheduler_interface {
public:
  default_scheduler_interface() = default;
  virtual ~default_scheduler_interface() = default;
  virtual void run(function_type f) {
    f();
  }
};

inline auto default_scheduler() {
  return scheduler(std::make_shared<default_scheduler_interface>());
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_default_scheduler__) */