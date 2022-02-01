
#if !defined(__another_rxcpp_h_source_base__)
#define __another_rxcpp_h_source_base__

#include <thread>
#include <vector>
#include "../../subscription.h"

namespace another_rxcpp { namespace internal {

class source_base : public std::enable_shared_from_this<source_base> {
public:
  enum class state {active, error, completed, unsubscribed};
  using sp = std::shared_ptr<source_base>;

private:
  std::mutex    mtx_;
  state         state_;
  subscription  subscription_; 

protected:
  sp shared_base() noexcept { return this->shared_from_this(); }
  void set_subscription(subscription sbsc) noexcept {
    subscription_ = std::move(sbsc);
  }

  void set_state_unsubscribed() noexcept {
    std::lock_guard<decltype(mtx_)> lock(mtx_);
    if(state_ == state::active){
      state_ = state::unsubscribed;
    }
  }

  virtual void unsubscribe_upstreams() noexcept = 0;

public:
  source_base() noexcept : state_(state::active) {}
  virtual ~source_base() = default;

  /** conditional emitters */
  void on_next_function(std::function<void()> f) noexcept {
    if(!subscription_.is_subscribed()) return;
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      return state_ == state::active;
    }();
    if(bExecute){
      f();
    }
  }

  void on_error_function(std::function<void()> f) noexcept {
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      if(state_ == state::active){
        state_ = state::error;
        return true;
      }
      return false;
    }();
    if(bExecute){
      f();
      unsubscribe();
    }
  }

  void on_completed_function(std::function<void()> f) noexcept {
    const bool bExecute = [&](){
      std::lock_guard<decltype(mtx_)> lock(mtx_);
      if(state_ == state::active){
        state_ = state::completed;
        return true;
      }
      return false;
    }();
    if(bExecute){
      f();
      unsubscribe();
    }
  }

  state state() const noexcept { return state_; }
  
  bool is_subscribed() const noexcept { return subscription_.is_subscribed(); }
  void unsubscribe() noexcept {
    unsubscribe_upstreams();
    subscription_.unsubscribe();
  }
};

}} /* namespace another_rxcpp::internal */

#endif /* !defined(__another_rxcpp_h_state_holder__) */