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
  const std::launch policy_;
  struct member {
    int               serial_;
    std::unordered_map<int, std::future<void>> items_;
    std::mutex mtx_;
  };
  std::shared_ptr<member> m_;



public:
  async_scheduler_interface(std::launch policy) :
    policy_(policy),
    m_(new member())
  {
    m_->serial_ = 0;
  };
  virtual ~async_scheduler_interface() = default;
  virtual void run(function_type f, any_sp_keeper keepalive) override {
    auto m = m_;
    std::lock_guard<std::mutex> lock(m->mtx_);
    auto serial = m->serial_++;
    m->items_.insert({
      serial,
      std::async(policy_, [keepalive, serial, m, f](){
        f();
        std::lock_guard<std::mutex> lock(m->mtx_);
        m->items_.erase(serial);
      })
    });
  }
};

inline auto async_scheduler(std::launch policy = std::launch::async) {
  return [policy]{
    return scheduler(std::make_shared<async_scheduler_interface>(policy));
  };
}

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_async_scheduler__) */