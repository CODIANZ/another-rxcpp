#if !defined(__another_rxcpp_h_default_scheduler__)
#define __another_rxcpp_h_default_scheduler__

#include "../scheduler.h"

namespace another_rxcpp {
namespace schedulers {

class default_scheduler_interface : public scheduler_interface {
public:
  default_scheduler_interface() noexcept :
    scheduler_interface(schedule_type::direct) {}
  virtual ~default_scheduler_interface() = default;

  virtual void run(call_in_context_fn_t call_in_context) noexcept override {
    call_in_context();
  }

  virtual void detach() noexcept override {
  }

  virtual void schedule(const function_type& f) noexcept override {
    f();
  }
};

inline auto default_scheduler() noexcept {
  return []{
    return scheduler(
      std::make_shared<default_scheduler_interface>()
    );
  };
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_default_scheduler__) */