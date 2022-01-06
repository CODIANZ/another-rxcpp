#if !defined(__h_new_thread_scheduler__)
#define __h_new_thread_scheduler__

#include "../scheduler.h"
#include <thread>
#include <queue>
#include <memory>

namespace another_rxcpp {
namespace schedulers {

class new_thread_scheduler_interface : public scheduler_interface
{
private:
  struct queue_item {
    function_type fn;
    any_sp_keeper keepalive;
  };
  struct member {
    std::queue<queue_item>  queue_;
    std::mutex              mtx_;
    std::thread             thread_;
    std::condition_variable cond_;
    bool                    bContinue_;
  };
  std::shared_ptr<member> m_;

public:
  new_thread_scheduler_interface() : m_(new member()) {
    m_->bContinue_ = true;
    auto m = m_;
    m->thread_ = std::thread([m](){
      while(true) {
        auto item = [m]() -> queue_item {
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m](){
              return !m->queue_.empty() || !m->bContinue_;
          });
          if(m->queue_.empty()) return {};
          auto it = m->queue_.front();
          m->queue_.pop();
          return it;
        }();
        if(!m->bContinue_) break;
        item.fn();
        item.keepalive.clear();
      }
    });
  }

  virtual ~new_thread_scheduler_interface(){
    /**
     * Conditions to come here
     * (1) all 'scheduler's that own this instance have been destroyed
     * (2) queue_ is empty (= lost keepalive)
     **/
    {
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->bContinue_ = false;
    }
    m_->cond_.notify_one();
    m_->thread_.detach();
  }

  virtual void run(function_type f, any_sp_keeper keepalive) override {
    {
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->queue_.push({f, keepalive});
    }
    m_->cond_.notify_one();
  }
};

inline auto new_thread_scheduler() {
  return [](){
    return scheduler(std::make_shared<new_thread_scheduler_interface>());
  };
}

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
inline auto observe_on_new_thread() {
  return new_thread_scheduler();
}
#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

} /* namespace schedulers */
} /* namespace another_rxcpp */

#endif /* !defined(__h_new_thread_scheduler__) */