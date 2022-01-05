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
  std::shared_ptr<member> m;

public:
  new_thread_scheduler_interface() : m(new member()) {
    m->bContinue_ = true;
    auto m_ = m;
    m->thread_ = std::thread([m_](){
      while(true) {
        auto item = [m_]() -> queue_item {
          std::unique_lock<std::mutex> lock(m_->mtx_);
          m_->cond_.wait(lock, [m_](){
              return !m_->queue_.empty() || !m_->bContinue_;
          });
          if(m_->queue_.empty()) return {};
          auto it = m_->queue_.front();
          m_->queue_.pop();
          return it;
        }();
        if(!m_->bContinue_) break;
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
      std::unique_lock<std::mutex> lock(m->mtx_);
      m->bContinue_ = false;
    }
    m->cond_.notify_one();
    m->thread_.detach();
  }

  virtual void run(function_type f, any_sp_keeper keepalive) override {
    {
      std::unique_lock<std::mutex> lock(m->mtx_);
      m->queue_.push({f, keepalive});
    }
    m->cond_.notify_one();
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