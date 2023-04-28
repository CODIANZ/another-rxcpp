#if !defined(__another_rxcpp_h_subject__)
#define __another_rxcpp_h_subject__

#include "../observable.h"
#include "../observables/connectable.h"
#include "../observables/error.h"
#include "../operators/on_error_resume_next.h"
#include "../operators/publish.h"
#include <algorithm>
#include <mutex>

namespace another_rxcpp {
namespace subjects {

template <typename T> class subject {
public:
  using value_type      = T;
  using source_type     = observables::connectable<value_type>;
  using observer_type   = observer<value_type>;
  using subscriber_type = subscriber<value_type>;

private:
  struct member {
    source_type         source_;
    subscriber_type     subscriber_;
    std::exception_ptr  error_ = nullptr;
    subscription        subscription_;
    std::recursive_mutex  mtx_;
    std::vector<internal::stream_controller<value_type>> sctls_;
    ~member() {
      subscription_.unsubscribe();
      std::lock_guard<std::recursive_mutex> lock(mtx_);
      std::for_each(sctls_.begin(), sctls_.end(), [](auto& sctl){
        sctl.finalize();
      });
    }
  };
  std::shared_ptr<member> m_;

protected:
  bool completed() const noexcept { return !m_->subscription_.is_subscribed(); }
  std::exception_ptr error() const noexcept { return m_->error_; }

public:
  subject() noexcept :
    m_(std::make_shared<member>())
  {
    std::weak_ptr<member> m = m_;
    m_->source_ = observable<>::create<value_type>([m](subscriber_type s){
      auto mm = m.lock();
      if(mm) mm->subscriber_ = s;
    })
    | operators::on_error_resume_next([m](std::exception_ptr err){
      auto mm = m.lock();
      if(mm) mm->error_ = err;
      return observables::error<value_type>(err);
    })
    | operators::publish();
    m_->subscription_ = m_->source_.connect();
  }

  virtual ~subject() = default;

  auto as_subscriber() const noexcept {
    return m_->subscriber_;
  }

  virtual observable<T> as_observable() const noexcept {
    auto m = m_;
    return observable<>::create<value_type>([m](subscriber_type s) {
      if(m->error_){
        s.on_error(m->error_);
        return;
      }
      else if(!m->subscription_.is_subscribed()){
        s.on_completed();
        return;
      }
      auto sctl = internal::stream_controller<value_type>(s);
      {
        std::lock_guard<std::recursive_mutex> lock(m->mtx_);
        m->sctls_.push_back(sctl);
      }
      m->source_.subscribe(sctl.template new_observer<value_type>(
        [sctl, m](auto, const value_type& x){
          sctl.sink_next(x);
        },
        [sctl, m](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl, m](auto serial) {
          sctl.sink_completed(serial);
        }
      ));
    });
  }



  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    auto get_subscriber() const { return as_subscriber(); }
    auto get_observable() const { return as_observable(); }
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */
};

} /* namespace subjects */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subject__) */
