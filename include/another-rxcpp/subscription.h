#if !defined(__another_rxcpp_h_subscription__)
#define __another_rxcpp_h_subscription__

#include <functional>
#include <memory>
#include <condition_variable>
#include "internal/tools/any_sp_keeper.h"

namespace another_rxcpp {

class subscription {
public:
  using is_subscribed_fn_t  = std::function<bool()>;
  using on_unsubscribe_fn_t = std::function<void()>;
  using cond_sp             = std::shared_ptr<std::condition_variable>;
  using any_sp_keeper       = internal::any_sp_keeper;

private:
  struct member {
    any_sp_keeper       sp_keeper_;
    is_subscribed_fn_t  is_subscribed_fn_;
    on_unsubscribe_fn_t on_unsubscribe_fn_;
    std::mutex          mtx_;
  };
  std::shared_ptr<member> m_;
  cond_sp                 cond_;

public:
  subscription() noexcept :
    cond_(std::make_shared<std::condition_variable>()) {}
  subscription(
    any_sp_keeper       sp_keeper,
    is_subscribed_fn_t  is_subscribed_fn,
    on_unsubscribe_fn_t on_unsubscribe_fn
  ) noexcept : 
    m_(std::make_shared<member>()),
    cond_(std::make_shared<std::condition_variable>())
  {
    m_->sp_keeper_        = sp_keeper;
    m_->is_subscribed_fn_ = is_subscribed_fn;
    m_->on_unsubscribe_fn_ = on_unsubscribe_fn;
  }
  ~subscription() = default;

  void unsubscribe() const noexcept {
    auto m = m_;
    if(!m) return;
    auto fn = [m](){
      std::lock_guard<std::mutex> lock(m->mtx_);
      auto f = m->on_unsubscribe_fn_;
      if(f){
        m->on_unsubscribe_fn_ = {};
        m->is_subscribed_fn_ = {};
      }
      return f;
    }();
    if(fn){
      fn();
      cond_->notify_all();
      m->sp_keeper_.clear();
    }
  }

  bool is_subscribed() const noexcept {
    auto m = m_;
    if(!m) return false;
    auto fn = [m](){
      std::lock_guard<std::mutex> lock(m->mtx_);
      return m->is_subscribed_fn_;
    }();

    return fn ? fn() : false;
  }

  auto unsubscribe_notice() const noexcept { return cond_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscription__) */
