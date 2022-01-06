#if !defined(__h_scheduler__)
#define __h_scheduler__

#include <memory>
#include <queue>

namespace another_rxcpp {

class scheduler_interface {
public:
  using function_type = std::function<void()>;
  scheduler_interface() = default;
  virtual ~scheduler_interface() = default;
  virtual void run(std::function<void()> call_in_context) = 0;
  virtual void detach() = 0;
};

class scheduler {
public:
  using function_type = typename scheduler_interface::function_type;
  using creator_fn    = std::function<scheduler()>;
  enum class type { async, sync };

private:
  using isp = std::shared_ptr<scheduler_interface>;

  struct member {
    std::queue<function_type> queue_;
    std::mutex                mtx_;
    std::condition_variable   cond_;
    isp                       interface_;
    int                       refcount_;
  };
  std::shared_ptr<member>     m_;

protected:

public:
  template <typename ISP> scheduler(ISP isp, type t)
  {
    if(t == type::async){
      m_ = std::make_shared<member>();
      auto m = m_;
      m->interface_ = std::dynamic_pointer_cast<scheduler_interface>(isp);
      m->refcount_  = 1;
      m->interface_->run([m](){
        while(true){
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock);
          while(!m->queue_.empty()){
            m->queue_.front()();
            m->queue_.pop();
          }
          if(m->refcount_ == 0) break;
        }
        m->interface_->detach();
      });
    }
  }

  scheduler(const scheduler& src) :
    m_(src.m_)
  {
    if(m_){
      std::lock_guard<std::mutex> lock(m_->mtx_);
      m_->refcount_++;
    }
  }

  virtual ~scheduler() {
    if(m_){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->refcount_--;
      if(m_->refcount_ == 0){
        lock.unlock();
        m_->cond_.notify_one();
      }
    }
  }

  void schedule(function_type f) const {
    if(m_){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->queue_.push(f);
      lock.unlock();
      m_->cond_.notify_one();
    }
    else{
      f();
    }
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_scheduler__) */