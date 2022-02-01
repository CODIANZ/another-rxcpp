#if !defined(__another_rxcpp_h_shared_with_will__)
#define __another_rxcpp_h_shared_with_will__

#include <memory>
#include <thread>
#include <functional>

namespace another_rxcpp { namespace internal {

template <typename T> class shared_with_will {
public:
  using element_type  = T;
  using element_sp    = std::shared_ptr<element_type>;
  using will_fn_t     = std::function<void(element_sp)>;

private:
  using refcount_sp = std::shared_ptr<std::atomic_size_t>;

  element_sp  element_;
  will_fn_t   will_fn_;
  refcount_sp refcount_;

public:
  shared_with_will(element_sp element, will_fn_t will_fn) noexcept :
    element_(element),
    will_fn_(will_fn),
    refcount_(std::make_shared<std::atomic_size_t>())
  {
    *refcount_ = 1;
  }

  shared_with_will(const shared_with_will& src) noexcept {
    *this = src;
  }

  shared_with_will& operator = (const shared_with_will& src) noexcept {
    (*src.refcount_)++;
    release();
    element_  = src.element_;
    will_fn_  = src.will_fn_;
    refcount_ = src.refcount_;
    return *this;
  }

  ~shared_with_will() noexcept {
    release();
  }

  element_sp operator-> () const noexcept { return element_; }

  element_sp capture_element() const noexcept { return element_; }

  void release() noexcept {
    if(!refcount_) return;
    const auto n = refcount_->fetch_sub(1);
    if(n == 1){
      if(will_fn_){
        will_fn_(element_);
        will_fn_ = {};
      }
    }
  }
};

}} /* namespace another_rxcpp::internal */

#endif /* !defined(__another_rxcpp_h_shared_with_will__) */