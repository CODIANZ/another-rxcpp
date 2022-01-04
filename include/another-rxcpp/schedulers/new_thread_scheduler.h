#if !defined(__h_new_thread_scheduler__)
#define __h_new_thread_scheduler__

#include "../scheduler.h"
#include <thread>
#include <queue>

namespace another_rxcpp {
namespace schedulers {

class new_thread_scheduler_interface : public scheduler_interface {
private:
  std::queue<function_type> queue_;
  std::mutex                mtx_;
  std::thread               thread_;
  std::condition_variable   cond_;
  volatile bool             bContinue_;

public:
  new_thread_scheduler_interface() : bContinue_(true) {
    thread_ = std::thread([&](){
      while(true) {
        auto f = [&]() -> function_type {
          std::unique_lock<std::mutex> lock(mtx_);
          cond_.wait(lock, [&](){
              return !queue_.empty() || !bContinue_;
          });
          if(queue_.empty()) return {};
          auto f = queue_.front();
          queue_.pop();
          return f;
        }();
        if(!bContinue_) break;
        f();
      }
    });
  }

  virtual ~new_thread_scheduler_interface(){
    bContinue_ = false;
    cond_.notify_one();
    thread_.join();
  }

  virtual void run(function_type f) {
    {
      std::unique_lock<std::mutex> lock(mtx_);
      queue_.push(f);
    }
    cond_.notify_one();
  }
};

inline auto new_thread_scheduler() {
  return scheduler(std::make_shared<new_thread_scheduler_interface>());
}

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
inline auto observe_on_new_thread() {
  return new_thread_scheduler();
}
#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_new_thread_scheduler__) */