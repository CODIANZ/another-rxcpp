#if !defined(__h_behavior__)
#define __h_behavior__

#include "../internal/source/source.h"
#include "../observables.h"
#include "../observable.h"
#include "subject.h"

namespace another_rxcpp {
namespace subjects {

template <typename T> class behavior : public subject<T> {
private:
  struct member {
    T          last_;
    std::mutex mtx_;
  };
  std::shared_ptr<member> m_;
  subscription    behavior_subscription_;

protected:

public:
  behavior(T initial_value) :
    m_(new member{
      .last_ = std::move(initial_value)
    })
  {
    auto m = m_; 
    behavior_subscription_ = as_observable().subscribe([m](T x){
      std::lock_guard<std::mutex> lock(m->mtx_);
      m->last_ = std::move(x);
    }, [](std::exception_ptr) {
    }, [](){
    });
  }

  virtual ~behavior(){
    behavior_subscription_.unsubscribe();
  }

  virtual observable<T> as_observable() const override {
    auto src = subject<T>::as_observable();
    auto m = m_;
    return observable<>::create<T>([src, m](subscriber<T> s){
      s.on_next([m](){
        std::lock_guard<std::mutex> lock(m->mtx_);
        return m->last_;
      }());
      src.subscribe({
        .on_next = [s](T x){
          s.on_next(std::move(x));
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
          s.on_completed();
        }
      });
    });
  }
};

} /* namespace subjects */
} /* namespace another_rxcpp */

#endif /* !defined(__h_behavior__) */
