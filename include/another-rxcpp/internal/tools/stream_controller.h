#if !defined(__another_rxcpp_h_stream_controller__)
#define __another_rxcpp_h_stream_controller__

#include <map>
#include <vector>
#include <mutex>
#include "fn.h"
#include "../../subscriber.h"

namespace another_rxcpp { namespace internal {

template <typename T> class stream_controller {
public:
  using value_type        = T;
  using subscriber_type   = subscriber<value_type>;
  using serial_type       = int32_t;
  using unsubscriber_map  = std::map<serial_type, internal::fn<void()>>;
  using on_finalize_t     = internal::fn<void()>;
  using on_finalizes_t    = std::vector<on_finalize_t>;

private:
  struct inner {
    subscriber_type       subscriber_;
    std::recursive_mutex  mtx_;
    serial_type           serial_;
    unsubscriber_map      unsubscribers_;
    on_finalizes_t        on_finalizes_;
    inner(const subscriber_type& sbsc) : subscriber_(sbsc) {}
  };

  mutable std::shared_ptr<inner> inner_;

  stream_controller(std::shared_ptr<inner> inner) noexcept
  : inner_(inner) {}

public:
  stream_controller(const subscriber_type& subscriber) noexcept {
    inner_ = std::make_shared<inner>(subscriber);
    subscriber.set_on_unsubscribe([inner = inner_]{
      stream_controller(inner).finalize();
    });
  }

  void set_on_finalize(const on_finalize_t& f) const noexcept {
    std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
    inner_->on_finalizes_.push(f);
  }

  void set_on_finalize(on_finalize_t&& f) const noexcept {
    std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
    inner_->on_finalizes_.push(std::move(f));
  }

  template <typename In> observer<In> new_observer(
    fn<void(serial_type, const In&)> n,
    fn<void(serial_type, std::exception_ptr)> e,
    fn<void(serial_type)> c
  ) const noexcept
  {
    const auto serial = [&]{
      std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
      return inner_->serial_++;
    }();

    auto ob = observer<In>(
      [n, serial](const In& value) {
        n(serial, value);
      },
      [e, serial](std::exception_ptr err) {
        e(serial, err);
      },
      [c, serial]() {
        c(serial);
      }
    );

    {
      std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);      
      inner_->unsubscribers_.insert({serial, [ob]{ ob.unsubscribe(); }});
    }
    return ob;
  }

  void sink_next(const value_type& value) const noexcept {
    if(inner_->subscriber_.is_subscribed()){
      inner_->subscriber_.on_next(value);
    }
    else{
      finalize();
    }
  }

  void sink_next(value_type&& value) const noexcept {
    if(inner_->subscriber_.is_subscribed()){
      inner_->subscriber_.on_next(std::move(value));
    }
    else{
      finalize();
    }
  }

  void sink_error(std::exception_ptr err) const noexcept {
    if(inner_->subscriber_.is_subscribed()){
      inner_->subscriber_.on_error(err);
      finalize();
    }
    else{
      finalize();
    }
  }

  void sink_completed(serial_type serial) const noexcept {
    if(inner_->subscriber_.is_subscribed()){
      const auto done_all = [&]{
        std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
        inner_->unsubscribers_.erase(serial);
        return inner_->unsubscribers_.size() == 0;
      }();
      if(done_all){
        inner_->subscriber_.on_completed();
        finalize();
      }
    }
    else{
      finalize();
    }
  }

  void sink_completed_force() const noexcept {
    if(inner_->subscriber_.is_subscribed()){
      inner_->subscriber_.on_completed();
    }
    finalize();
  }

  void upstream_abort_observe(serial_type serial) const noexcept {
    const auto f = [&]{
      std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
      auto it = inner_->unsubscribers_.find(serial);
      if(it == inner_->unsubscribers_.end()){
        return on_finalize_t();
      }
      auto f = it->second;
      inner_->unsubscribers_.erase(it);
      return f;
    }();
    if(f){
      f();
    }
  }

  void finalize() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
    for(auto it = inner_->unsubscribers_.begin(); it != inner_->unsubscribers_.end(); it++){
      it->second();
    }
    inner_->unsubscribers_.clear();
    if(inner_->subscriber_.is_subscribed()){
      inner_->subscriber_.unsubscribe();
    }
    for(auto it = inner_->on_finalizes_.begin(); it != inner_->on_finalizes_.end(); it++){
      (*it)();
    }
    inner_->on_finalizes_.clear();
  }

  bool is_subscribed() const noexcept {
    return inner_->subscriber_.is_subscribed();
  }
};

}} /* namespace another_rxcpp::internal */
#endif /* !defined(__another_rxcpp_h_stream_controller__) */
