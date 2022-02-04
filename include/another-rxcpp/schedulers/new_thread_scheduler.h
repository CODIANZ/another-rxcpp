#if !defined(__another_rxcpp_h_new_thread_scheduler__)
#define __another_rxcpp_h_new_thread_scheduler__

#include "../scheduler.h"
#include <thread>
#include <queue>
#include <memory>

namespace another_rxcpp {
namespace schedulers {

class new_thread_scheduler_interface : public scheduler_interface
{
private:
  std::thread thread_;

public:
  new_thread_scheduler_interface() noexcept :
    scheduler_interface(schedule_type::queuing) {}

  virtual ~new_thread_scheduler_interface() = default;

  virtual void run(call_in_context_fn_t call_in_context) noexcept override {
    thread_ = std::thread([call_in_context](){
      call_in_context();
    });
  }

  virtual void detach() noexcept override {
    thread_.detach();
  }

  virtual void schedule(const function_type& f) noexcept override {
    f();
  }
};

inline auto new_thread_scheduler() {
  return [](){
    return scheduler(
      std::make_shared<new_thread_scheduler_interface>()
    );
  };
}

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
inline auto observe_on_new_thread() {
  return new_thread_scheduler();
}
#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_new_thread_scheduler__) */