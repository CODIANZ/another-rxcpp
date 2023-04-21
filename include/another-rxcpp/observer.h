#if !defined(__another_rxcpp_h_observer__)
#define __another_rxcpp_h_observer__

#include <functional>
#include <memory>
#include <optional>
#include "internal/tools/fn.h"
#include "internal/tools/util.h"

namespace another_rxcpp {

template <typename T = void> struct observer;

namespace internal {
  template <typename T> class stream_controller;
}

template <> struct observer<void> {
  template <typename T> using next_t = internal::fn<void(const T&)>;
  using error_t       = internal::fn<void(std::exception_ptr)>;
  using completed_t   = internal::fn<void()>;
  using unsubscribe_t = internal::fn<void()>;
};

template <typename T> struct observer {
  using value_type     = T;
  using next_t         = observer<>::next_t<value_type>;
  using error_t        = observer<>::error_t;
  using completed_t    = observer<>::completed_t;
  using unsubscribe_t  = observer<>::unsubscribe_t;

  using next_sp        = std::shared_ptr<next_t>;
  using error_sp       = std::shared_ptr<error_t>;
  using completed_sp   = std::shared_ptr<completed_t>;
  using unsubscribe_sp = std::shared_ptr<unsubscribe_t>;

private:
  struct inner {
    next_sp        next_;
    error_sp       error_;
    completed_sp   completed_;
    unsubscribe_sp unsubscribe_;
  };

  mutable std::shared_ptr<inner> inner_;

  friend class internal::stream_controller<T>;

  void set_on_unsubscribe(const unsubscribe_t& f) const noexcept {
    inner_->unsubscribe_ = internal::to_shared(f);
  }

  void set_on_unsubscribe(unsubscribe_t&& f) const noexcept {
    inner_->unsubscribe_ = internal::to_shared(std::move(f));
  }


public:
  observer(const next_t& n = {}, const error_t& e = {}, const completed_t& c = {}) noexcept {
    inner_ = std::make_shared<inner>();
    inner_->next_       = internal::to_shared(n);
    inner_->error_      = internal::to_shared(e);
    inner_->completed_  = internal::to_shared(c);
  }

  observer(next_t&& n, error_t&& e, completed_t&& c) noexcept {
    inner_ = std::make_shared<inner>();
    inner_->next_       = internal::to_shared(std::move(n));
    inner_->error_      = internal::to_shared(std::move(e));
    inner_->completed_  = internal::to_shared(std::move(c));
  }

  void unsubscribe() const noexcept {
    auto unsb = inner_->unsubscribe_;

    inner_->next_.reset();
    inner_->error_.reset();
    inner_->completed_.reset();
    inner_->unsubscribe_.reset();
    
    if(unsb && *unsb) {
      (*unsb)();
    }
  }

  bool is_subscribed() const noexcept {
    return inner_->next_ && inner_->error_ && inner_->completed_;
  }

  void on_next(const value_type& value) const noexcept {
    auto n = inner_->next_;
    if(n && *n) (*n)(value);
  }

  void on_next(value_type&& value) const noexcept {
    auto n = inner_->next_;
    if(n && *n) (*n)(std::move(value));
  }

  void on_error(std::exception_ptr err) const noexcept {
    auto e = inner_->error_;
    auto u = inner_->unsubscribe_;
    unsubscribe();
    if(e && *e) (*e)(err);
    if(u && *u) (*u)();
  }

  void on_completed() const noexcept {
    auto c = inner_->completed_;
    auto u = inner_->unsubscribe_;
    unsubscribe();
    if(c && *c) (*c)();
    if(u && *u) (*u)();
  }
};

} /* namespace another_rxcpp */
#endif /* !defined(__another_rxcpp_h_observable__) */
