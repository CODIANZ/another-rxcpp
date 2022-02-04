#if !defined(__another_rxcpp_h_async_scheduler__)
#define __another_rxcpp_h_async_scheduler__

#include "../scheduler.h"
#include <future>
#include <memory>
#include <vector>
#include <unordered_map>

namespace another_rxcpp {
namespace schedulers {

class async_scheduler_interface : public scheduler_interface {
private:
  std::future<void> future_;

public:
  async_scheduler_interface() noexcept :
    scheduler_interface(schedule_type::queuing) {}
  virtual ~async_scheduler_interface() = default;

  virtual void run(call_in_context_fn_t call_in_context) noexcept override {
    future_ = std::async(std::launch::async, [call_in_context](){
      call_in_context();
    });
  }

  virtual void detach() noexcept override {
    future_ = {};
  }

  virtual void schedule(const function_type& f) noexcept override {
    f();
  }
};

inline auto async_scheduler() noexcept {
  return []{
    return scheduler(
      std::make_shared<async_scheduler_interface>()
    );
  };
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_async_scheduler__) */