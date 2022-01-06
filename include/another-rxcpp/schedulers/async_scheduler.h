#if !defined(__h_async_scheduler__)
#define __h_async_scheduler__

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
  async_scheduler_interface() = default;
  virtual ~async_scheduler_interface() = default;

  virtual void run(std::function<void()> call_in_context) override {
    future_ = std::async(std::launch::async, [call_in_context](){
      call_in_context();
    });
  }

  virtual void detach() override {
    /* nothing to do */
  }
};

inline auto async_scheduler() {
  return []{
    return scheduler(
      std::make_shared<async_scheduler_interface>(),
      scheduler::type::async
    );
  };
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_async_scheduler__) */