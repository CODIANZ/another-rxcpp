
#if !defined(__h_source_base__)
#define __h_source_base__

#include <thread>
#include <vector>
#include "subscription.h"

namespace another_rxcpp {

class source_base : public std::enable_shared_from_this<source_base> {
public:
  enum class state {active, error, completed};
  using sp = std::shared_ptr<source_base>;

private:
  std::mutex      mtx_;
  state           state_;
  source_base::sp upstream_; 
  std::vector<subscription>   subscriptions_;

protected:
  void add_subscription(subscription s) {
    std::lock_guard<decltype(mtx_)> lock(mtx_);
    subscriptions_.push_back(s);
  }

   void remove_subscriptions() {
    {
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      subscriptions_.clear();
    }
    if(upstream_){
      upstream_->remove_subscriptions();
      upstream_.reset();
    }
  }

  sp shared_base() { return this->shared_from_this(); }

public:
  source_base(source_base::sp upstream = {}) :
    state_(state::active), upstream_(upstream) {}
  virtual ~source_base() = default;

  /** conditional emitters */
  void on_next_function(std::function<void()> f){
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
      remove_subscriptions();
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
      remove_subscriptions();
      f();
    }
  }

  state state() const { return state_; }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_state_holder__) */