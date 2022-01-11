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

private:
  struct member {
    any_sp_keeper       sp_keeper_;
    is_subscribed_fn_t  is_subscribed_fn_;
    on_unsubscribe_fn_t on_unsubscribe_fn_;
    cond_sp             cond_;
    std::mutex          mtx_;
  };
  std::shared_ptr<member> m_;

public:
  subscription() : m_(std::make_shared<member>()) {
    m_->cond_ = std::make_shared<std::condition_variable>();
  }
  subscription(
    any_sp_keeper       sp_keeper,
    is_subscribed_fn_t  is_subscribed_fn,
    on_unsubscribe_fn_t on_unsubscribe_fn
  ) : m_(std::make_shared<member>())
  {
    m_->sp_keeper_ = sp_keeper;
    m_->is_subscribed_fn_ = is_subscribed_fn;
    m_->on_unsubscribe_fn_ = on_unsubscribe_fn;
    m_->cond_ = std::make_shared<std::condition_variable>();
  }
  ~subscription() = default;

  void unsubscribe() const {
    auto fn = [&](){
      std::lock_guard<std::mutex> lock(m_->mtx_);
      auto f = m_->on_unsubscribe_fn_;
      if(f){
        m_->on_unsubscribe_fn_ = {};
        m_->is_subscribed_fn_ = {};
      }
      return f;
    }();
    if(fn){
      fn();
      //m_->sp_keeper_.clear();
      m_->cond_->notify_all();
    }
  }

  bool is_subscribed() const {
    auto fn = [&](){
      std::lock_guard<std::mutex> lock(m_->mtx_);
      return m_->is_subscribed_fn_;
    }();

    return fn ? fn() : false;
  }

  auto unsubscribe_notice() const { return m_->cond_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscription__) */
