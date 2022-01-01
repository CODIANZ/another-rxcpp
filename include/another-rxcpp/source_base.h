
#if !defined(__h_source_base__)
#define __h_source_base__

#include <thread>
#include <vector>
#include "subscription.h"

namespace another_rxcpp {

class source_base : public std::enable_shared_from_this<source_base> {
public:
  enum class state {active, error, completed, unsubscribed};
  using sp = std::shared_ptr<source_base>;

private:
  std::mutex    mtx_;
  state         state_;
  subscription  subscription_; 

protected:
  sp shared_base() { return this->shared_from_this(); }
  void set_subscription(subscription sbsc) {
    subscription_ = std::move(sbsc);
  }

  void set_state_unsubscribed() {
    std::lock_guard<decltype(mtx_)> lock(mtx_);
    if(state_ == state::active){
      state_ = state::unsubscribed;
    }
  }

public:
  source_base(source_base::sp upstream = {}) :
    state_(state::active) {}
  virtual ~source_base() = default;

  /** conditional emitters */
  void on_next_function(std::function<void()> f){
    if(!subscription_.is_subscribed()) return;
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      return state_ == state::active;
    }();
    if(bExecute){
      f();
    }
  }

  void on_error_function(std::function<void()> f){
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      if(state_ == state::active){
        state_ = state::error;
        return true;
      }
      return false;
    }();
    if(bExecute){
      subscription_.unsubscribe();
      f();
    }
  }

  void on_completed_function(std::function<void()> f){
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      if(state_ == state::active){
        state_ = state::completed;
        return true;
      }
      return false;
    }();
    if(bExecute){
      subscription_.unsubscribe();
      f();
    }
  }

  state state() const { return state_; }
  
  bool is_subscribed() const { return subscription_.is_subscribed(); }
  void unsubscribe() { return subscription_.unsubscribe(); }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_state_holder__) */