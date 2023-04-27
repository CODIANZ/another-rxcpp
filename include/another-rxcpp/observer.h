#if !defined(__another_rxcpp_h_observer__)
#define __another_rxcpp_h_observer__

#include <functional>
#include <memory>
#include <mutex>
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
    std::recursive_mutex  mtx_;
    next_sp        next_;
    error_sp       error_;
    completed_sp   completed_;
    unsubscribe_sp unsubscribe_;
  };

  mutable std::shared_ptr<inner> inner_;

  friend class internal::stream_controller<T>;

  template <typename Unsb> void set_on_unsubscribe(Unsb&& f) const noexcept {
    inner_->unsubscribe_ = std::make_shared<unsubscribe_t>(std::forward<Unsb>(f));
  }

  auto fetch_and_reset_all() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(inner_->mtx_);
    auto e = inner_->error_;
    auto c = inner_->completed_;
    auto u = inner_->unsubscribe_;
    inner_->next_.reset();
    inner_->error_.reset();
    inner_->completed_.reset();
    inner_->unsubscribe_.reset();
    return std::make_tuple(e, c, u);
  }

public:
  template<typename N, typename E, typename C> observer(N&& n, E&& e, C&& c) noexcept {
    inner_ = std::make_shared<inner>();
    inner_->next_       = std::make_shared<next_t>(std::forward<N>(n));
    inner_->error_      = std::make_shared<error_t>(std::forward<E>(e));
    inner_->completed_  = std::make_shared<completed_t>(std::forward<C>(c));
  }

  observer(const next_t& n = {}, const error_t& e = {}, const completed_t& c = {}) noexcept {
    inner_ = std::make_shared<inner>();
    inner_->next_       = std::make_shared<next_t>(n);
    inner_->error_      = std::make_shared<error_t>(e);
    inner_->completed_  = std::make_shared<completed_t>(c);
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
    auto ecu = fetch_and_reset_all();
    auto e = std::get<0>(ecu);
    auto u = std::get<2>(ecu);
    if(e && *e) (*e)(err);
    if(u && *u) (*u)();
  }

  void on_completed() const noexcept {
    auto ecu = fetch_and_reset_all();
    auto c = std::get<1>(ecu);
    auto u = std::get<2>(ecu);
    if(c && *c) (*c)();
    if(u && *u) (*u)();
  }

  void unsubscribe() const noexcept {
    auto ecu = fetch_and_reset_all();
    auto u = std::get<2>(ecu);
    if(u && *u) (*u)();
  }

  bool is_subscribed() const noexcept {
    return inner_->next_ && inner_->error_ && inner_->completed_;
  }
};

} /* namespace another_rxcpp */
#endif /* !defined(__another_rxcpp_h_observable__) */
