#if !defined(__h_shared_with_will__)
#define __h_shared_with_will__

#include <memory>
#include <thread>
#include <functional>

template <typename T> class shared_with_will {
public:
  using element_type  = T;
  using element_sp    = std::shared_ptr<element_type>;
  using will_fn_t     = std::function<void(element_sp)>;

private:
  struct refinfo {
    std::mutex  mtx_;
    std::size_t refcount_;
  };
  using refinfo_sp = std::shared_ptr<refinfo>;

  element_sp  element_;
  will_fn_t   will_fn_;
  refinfo_sp  refinfo_;

  void release(){
    if(!refinfo_) return;
    std::lock_guard<std::mutex> lock(refinfo_->mtx_);
    refinfo_->refcount_--;
    if(refinfo_->refcount_ == 0){
      if(will_fn_) will_fn_(element_);
    }
    element_.reset();
    refinfo_.reset();
    will_fn_ = {};
  }

public:
  shared_with_will(element_sp element, will_fn_t will_fn) :
    element_(element),
    will_fn_(will_fn),
    refinfo_(std::make_shared<refinfo>())
  {
    refinfo_->refcount_ = 1;
  }

  shared_with_will(const shared_with_will& src){
    *this = src;
  }

  shared_with_will& operator = (const shared_with_will& src){
    release();
    std::lock_guard<std::mutex> lock(src.refinfo_->mtx_);
    element_  = src.element_;
    refinfo_  = src.refinfo_;
    will_fn_  = src.will_fn_;
    refinfo_->refcount_++;
    return *this;
  }

  ~shared_with_will(){
    release();
  }

  element_sp operator-> () const { return element_; }

  element_sp capture_element() const { return element_; }
};


#endif /* !defined(__h_shared_with_will__) */