#if !defined(__another_rxcpp_h_behavior__)
#define __another_rxcpp_h_behavior__

#include "../internal/source/source.h"
#include "../internal/tools/shared_with_will.h"
#include "../observables.h"
#include "../observable.h"
#include "subject.h"

namespace another_rxcpp {
namespace subjects {

template <typename T> class behavior : public subject<T> {
private:
  struct member {
    T               last_;
    std::mutex      mtx_;
    subscription    subscription_;
    member(const T& last) : last_(last) {}
  };
  shared_with_will<member> m_;

protected:

public:
  behavior(T initial_value) :
    m_(std::make_shared<member>(std::move(initial_value)), [](auto x){
      x->subscription_.unsubscribe();
    })
  {
    auto m = m_.capture_element();
    m->subscription_ = as_observable().subscribe([m](T x){
      std::lock_guard<std::mutex> lock(m->mtx_);
      m->last_ = std::move(x);
    }, [](std::exception_ptr) {
    }, [](){
    });
  }

  virtual ~behavior() = default;

  virtual observable<T> as_observable() const override {
    auto src = subject<T>::as_observable();
    auto m = m_.capture_element();
    auto base_completed = subject<T>::completed();
    auto base_error = subject<T>::error();
    return observable<>::create<T>([src, m, base_completed, base_error](subscriber<T> s){
      if(base_completed){
        if(base_error != nullptr){
          s.on_error(base_error);
        }
        else{
          s.on_completed();
        }
      }
      else{
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
      }
    });
  }
};

} /* namespace subjects */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_behavior__) */
