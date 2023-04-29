#if !defined(__another_rxcpp_h_scheduler__)
#define __another_rxcpp_h_scheduler__

#include "internal/tools/fn.h"
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace another_rxcpp {

class scheduler_interface {
public:
  enum class schedule_type {direct, queuing};
  using function_type = internal::fn<void()>;
  using call_in_context_fn_t = internal::fn<void()>;

private:
  schedule_type schedule_type_;

protected:
  scheduler_interface(schedule_type stype) noexcept : schedule_type_(stype) {}

public:
  virtual ~scheduler_interface() = default;

  schedule_type get_schedule_type() const noexcept { return schedule_type_; }

  virtual void run(call_in_context_fn_t call_in_context) noexcept = 0;
  virtual void schedule(const function_type& f) noexcept = 0;

  /**
   * `detach()` is called in the same thread as the scheduler's `run()`.
   * Derived schedulers must therefore be prepared to safely dispose of the execution context.
   * (This causes 'valgrind' to detect 'possibly-lost'.)
  */
  virtual void detach() noexcept = 0;
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
    bool                      abort_ = false;
  };
  std::shared_ptr<member>     m_;

public:
  template <typename ISP> scheduler(ISP isp) noexcept :
    m_(std::make_shared<member>())
  {
    m_->interface_ = std::dynamic_pointer_cast<scheduler_interface>(isp);

    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::weak_ptr<member> m = m_;
      m_->interface_->run([m](){
        while(true){
          auto q = [m]() -> std::queue<function_type> {
            auto mm = m.lock();
            if(!mm) {
              return std::queue<function_type>();
            }
            std::unique_lock<std::mutex> lock(mm->mtx_);
            mm->cond_.wait(lock, [mm]{ return !mm->queue_.empty() || mm->abort_; });
            if(mm->abort_){
              return std::queue<function_type>();
            }
            return std::move(mm->queue_);
          }();
          if(q.empty()) break;
          while(!q.empty()){
            auto mm = m.lock();
            if(!mm) break;
            mm->interface_->schedule(q.front());
            q.pop();
          }
        }
      });
    }
  }

  scheduler(const scheduler& src) noexcept {
    *this = src;
  }

  scheduler& operator= (const scheduler& src) noexcept {
    m_ = src.m_;
    return *this;
  }

  virtual ~scheduler() noexcept {
  }

  template <typename F> void schedule(F&& f) const noexcept {
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->queue_.push(std::forward<F>(f));
      lock.unlock();
      m_->cond_.notify_one();
    }
    else{
      /* m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::direct */
      m_->interface_->schedule(std::forward<F>(f));
    }
  }

  void abort() const noexcept {
      if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
        {
          std::unique_lock<std::mutex> lock(m_->mtx_);
          m_->abort_ = true;
        }
        m_->cond_.notify_one();
        m_->interface_->detach();
      }
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_scheduler__) */
