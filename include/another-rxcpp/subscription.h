#if !defined(__h_subscription__)
#define __h_subscription__

#include <functional>
#include <memory>
#include <condition_variable>
#include "internal/tools/any_sp_keeper.h"

namespace another_rxcpp {

class source_base;
class subscription {
public:
  using is_subscribed_fn_t  = std::function<bool()>;
  using on_unsubscribe_fn_t = std::function<void()>;

private:
  any_sp_keeper       sp_keeper_;
  is_subscribed_fn_t  is_subscribed_fn_;
  on_unsubscribe_fn_t on_unsubscribe_fn_;
  std::shared_ptr<std::condition_variable> cond_;

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

  void unsubscribe() {
    if(on_unsubscribe_fn_){
      on_unsubscribe_fn_();
      cond_->notify_all();
    }
    on_unsubscribe_fn_ = {};
  }

  bool is_subscribed() const {
    return is_subscribed_fn_ ? is_subscribed_fn_() : false;
  }

  auto unsubscribe_notice() { return cond_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_subscription__) */
