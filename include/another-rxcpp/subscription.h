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
  mutable any_sp_keeper       sp_keeper_;
  mutable is_subscribed_fn_t  is_subscribed_fn_;
  mutable on_unsubscribe_fn_t on_unsubscribe_fn_;
  mutable cond_sp             cond_;

public:
  subscription() = default;
  subscription(
    any_sp_keeper       sp_keeper,
    is_subscribed_fn_t  is_subscribed_fn,
    on_unsubscribe_fn_t on_unsubscribe_fn
  ) :
    sp_keeper_(sp_keeper),
    is_subscribed_fn_(is_subscribed_fn),
    on_unsubscribe_fn_(on_unsubscribe_fn),
    cond_(std::make_shared<std::condition_variable>()) {};
  ~subscription() = default;

  void unsubscribe() const {
    if(on_unsubscribe_fn_){
      on_unsubscribe_fn_();
      cond_->notify_all();
    }
    on_unsubscribe_fn_ = {};
    sp_keeper_.clear();
  }

  bool is_subscribed() const {
    return is_subscribed_fn_ ? is_subscribed_fn_() : false;
  }

  auto unsubscribe_notice() const { return cond_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscription__) */
