#if !defined(__h_subscription__)
#define __h_subscription__

#include <functional>
#include <memory>
#include <condition_variable>

namespace another_rxcpp {

class subscription {
public:
  using discard_fn_t        = std::function<void()>;
  using is_subscribed_fn_t  = std::function<bool()>;

private:
  discard_fn_t        discard_fn_;
  is_subscribed_fn_t  is_subscribed_fn_;
  std::shared_ptr<std::condition_variable> cond_;

public:
  subscription() = default;
  subscription(discard_fn_t discard_fn, is_subscribed_fn_t is_subscribed_fn) :
    discard_fn_(discard_fn),
    is_subscribed_fn_(is_subscribed_fn),
    cond_(std::make_shared<std::condition_variable>()) {};
  ~subscription() = default;

  void unsubscribe(){
    if(discard_fn_){
      discard_fn_();
      cond_->notify_all();
    }
    discard_fn_ = {};
  }

  bool is_subscribed() const {
    return is_subscribed_fn_ ? is_subscribed_fn_() : false;
  }

  auto unsubscribe_notice() { return cond_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_subscription__) */
