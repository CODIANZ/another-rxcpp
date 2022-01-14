#if !defined(__another_rxcpp_h_subject__)
#define __another_rxcpp_h_subject__

#include "../internal/source/source.h"
#include "../internal/tools/shared_with_will.h"
#include "../observable.h"
#include "../observables/connectable.h"
#include "../operators/on_error_resume_next.h"
#include "../operators/publish.h"

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
    std::exception_ptr  error_;
    subscription        subscription_;
  };
  internal::shared_with_will<member> m_;

protected:
  bool completed() const { return !m_->subscription_.is_subscribed(); }
  std::exception_ptr error() const { return m_->error_; }

public:
  subject() :
    m_(std::make_shared<member>(), [](auto x){
      x->subscription_.unsubscribe();
    })
  {
    auto wm = internal::to_weak(m_.capture_element());
    m_->source_ = observable<>::create<value_type>([wm](subscriber_type s){
      auto m = wm.lock();
      if(m) m->subscriber_ = s;
    })
    | operators::on_error_resume_next([wm](std::exception_ptr err){
      auto m = wm.lock();
      if(m) m->error_ = err;
      return observables::error<value_type>(err);
    })
    | operators::publish();
    m_->subscription_ = m_->source_.connect();
  }

  virtual ~subject() = default;

  auto as_subscriber() const {
    return m_->subscriber_;
  }

  // virtual observable<T> as_observable() const {
  //   auto mm = m_;
  //   return observable<>::create<value_type>([mm](subscriber_type s) mutable {
  //     auto wm = to_weak(mm.capture_element());
  //     auto m = wm.lock();
  //     mm.release();
  //     if(m->error_){
  //       s.on_error(m->error_);
  //     }
  //     else if(!m->subscription_.is_subscribed()){
  //       s.on_completed();
  //     }
  //     m->source_.subscribe({
  //       .on_next = [s](value_type x){
  //         s.on_next(std::move(x));
  //       },
  //       .on_error = [s](std::exception_ptr err) mutable {
  //         s.on_error(err);
  //       },
  //       .on_completed = [s]() mutable {
  //         s.on_completed();
  //       },
  //     });
  //   });
  // }


  virtual observable<T> as_observable() const {
    auto m = m_;
    return observable<>::create<value_type>([m](subscriber_type s) mutable {
      auto mm = m.capture_element();
      m.release();
      if(mm->error_){
        s.on_error(m->error_);
      }
      else if(!mm->subscription_.is_subscribed()){
        s.on_completed();
      }
      mm->source_.subscribe({
        .on_next = [s](value_type x){
          s.on_next(std::move(x));
        },
        .on_error = [s](std::exception_ptr err) mutable {
          s.on_error(err);
        },
        .on_completed = [s]() mutable {
          s.on_completed();
        },
      });
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
