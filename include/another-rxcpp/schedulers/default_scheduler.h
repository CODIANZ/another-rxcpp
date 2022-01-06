#if !defined(__h_default_scheduler__)
#define __h_default_scheduler__

#include "../scheduler.h"

namespace another_rxcpp {
namespace schedulers {

class default_scheduler_interface : public scheduler_interface {
public:
  default_scheduler_interface() = default;
  virtual ~default_scheduler_interface() = default;

  virtual void run(std::function<void()> call_in_context) override {
    call_in_context();
  }

  virtual void detach() override {
  }
};

inline auto default_scheduler() {
  return []{
    return scheduler(
      std::make_shared<default_scheduler_interface>(),
      scheduler::type::sync
    );
  };
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_default_scheduler__) */