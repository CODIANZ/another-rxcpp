#if !defined(__h_subject__)
#define __h_subject__

#include "../internal/source/source.h"
#include "../internal/tools/shared_with_will.h"
#include "../observables.h"
#include "../observable.h"
#include "../operators.h"

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
  shared_with_will<member> m_;

protected:
  bool completed() const { return !m_->subscription_.is_subscribed(); }
  std::exception_ptr error() const { return m_->error_; }

public:
  subject() :
    m_(std::make_shared<member>(), [](auto x){
      x->subscription_.unsubscribe();
    })
  {
    auto m = m_.capture_element();
    m->source_ = observable<>::create<value_type>([m](subscriber_type s){
      m->subscriber_ = s;
    })
    | operators::tap<value_type>({
      .on_error = [m](std::exception_ptr err){
        m->error_ = err;
      }
    })
    | operators::publish();
    m->subscription_ = m->source_.connect();
  }

  virtual ~subject() = default;

  auto as_subscriber() const {
    return m_->subscriber_;
  }

  virtual observable<T> as_observable() const {
    auto m = m_.capture_element();
    return observable<>::create<value_type>([m](subscriber_type s){
      if(m->error_){
        s.on_error(m->error_);
      }
      else if(!m->subscription_.is_subscribed()){
        s.on_completed();
      }
      m->source_.subscribe({
        .on_next = [s](value_type x){
          s.on_next(std::move(x));
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
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

#endif /* !defined(__h_subject__) */
