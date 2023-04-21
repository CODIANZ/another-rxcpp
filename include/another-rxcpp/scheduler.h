#if !defined(__another_rxcpp_h_scheduler__)
#define __another_rxcpp_h_scheduler__

#include "internal/tools/fn.h"
#include <memory>
#include <queue>
#include <mutex>

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
  virtual void detach() noexcept = 0;
  virtual void schedule(const function_type& f) noexcept = 0;
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
      auto m = m_;
      m->interface_->run([m](){
        while(true){
          auto q = [m]() -> std::queue<function_type> {
            std::unique_lock<std::mutex> lock(m->mtx_);
            m->cond_.wait(lock, [m]{ return !m->queue_.empty() || m->abort_; });
            if(m->abort_){
              return std::queue<function_type>();
            }
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

  scheduler(const scheduler& src) noexcept {
    *this = src;
  }

  scheduler& operator= (const scheduler& src) noexcept {
    m_ = src.m_;
    return *this;
  }

  virtual ~scheduler() noexcept {
  }

  void schedule(const function_type& f) const noexcept {
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      auto cpf = f;
      schedule(std::move(f));
    }
    else{
      /* m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::direct */
      m_->interface_->schedule(f);
    }
  }

  void schedule(function_type&& f) const noexcept {
    if(m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::queuing){
      std::unique_lock<std::mutex> lock(m_->mtx_);
      m_->queue_.push(std::move(f));
      lock.unlock();
      m_->cond_.notify_one();
    }
    else{
      /* m_->interface_->get_schedule_type() == scheduler_interface::schedule_type::direct */
      m_->interface_->schedule(std::move(f));
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
