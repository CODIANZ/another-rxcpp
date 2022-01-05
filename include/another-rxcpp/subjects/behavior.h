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
  T last_;

protected:

public:
  behavior(T initial_value) : last_(std::move(initial_value)) {}

  virtual ~behavior() = default;

  virtual observable<T> as_observable() const override {
    return observable<>::create<T>([=](subscriber<T> s){
      auto src = subject<T>::as_observable();
      s.on_next(last_);
      src.subscribe({
        .on_next = [s](auto&& x){
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
