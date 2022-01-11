#if !defined(__another_rxcpp_h_scheduler__)
#define __another_rxcpp_h_scheduler__

#include <memory>
#include <queue>

namespace another_rxcpp {

class scheduler_interface {
public:
  enum class schedule_type {direct, queuing};
  using function_type = std::function<void()>;
  using call_in_context_fn_t = std::function<void()>;

private:
  schedule_type schedule_type_;

protected:
  scheduler_interface(schedule_type stype) : schedule_type_(stype) {}

public:
  virtual ~scheduler_interface() = default;

  schedule_type get_schedule_type() const { return schedule_type_; }

  virtual void run(call_in_context_fn_t call_in_context) = 0;
  virtual void detach() = 0;
  virtual void schedule(function_type f) = 0;
};

class scheduler {
public:
  using function_type = typename scheduler_interface::function_type;
  using creator_fn    = std::function<scheduler()>;
  enum class interface_type { direct, queuing };

private:
  using isp = std::shared_ptr<scheduler_interface>;

  struct member {
    isp                       interface_;
    std::mutex                mtx_;
    std::condition_variable   cond_;
    std::queue<function_type> queue_;
    int                       refcount_;
  };
  std::shared_ptr<member>     m_;

  void release() {
    if(!m_) return;
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->refcount_--;
      if(m_->refcount_ == 0){
        lock.unlock();
        m_->cond_.notify_one();
        m_->interface_->detach();
      }
    }
    m_.reset();
  }

public:
  template <typename ISP> scheduler(ISP isp) :
    m_(std::make_shared<member>())
  {
    m_->interface_ = std::dynamic_pointer_cast<scheduler_interface>(isp);

    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      auto m = m_;
      m->refcount_  = 1;
      m->interface_->run([m](){
        while(true){
          auto q = [m]() -> std::queue<function_type> {
            std::unique_lock<std::mutex> lock(m->mtx_);
            m->cond_.wait(lock, [m]{ return !m->queue_.empty() || m->refcount_ == 0; });
            return std::move(m->queue_);
          }();
          if(q.empty()) break;
          while(!q.empty()){
            m->interface_->schedule(q.front());
            q.pop();
          }
        }
      });
    }
  }

  scheduler(const scheduler& src){
    *this = src;
  }

  scheduler& operator= (const scheduler& src) {
    release();
    m_ = src.m_;
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::lock_guard<std::mutex> lock(m_->mtx_);
      m_->refcount_++;    
    }
    return *this;
  }

  virtual ~scheduler() {
    release();
  }

  void schedule(function_type f) const {
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->queue_.push(f);
      lock.unlock();
      m_->cond_.notify_one();
    }
    else{
      /* m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::direct */
      m_->interface_->schedule(f);
    }
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_scheduler__) */