#if !defined(__another_rxcpp_h_behavior__)
#define __another_rxcpp_h_behavior__

#include "../internal/tools/stream_controller.h"
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
    ~member() {
      subscription_.unsubscribe();
    }
  };
  std::shared_ptr<member> m_;

  void initialize() {
    auto m = m_;
    m->subscription_ = as_observable().subscribe({
      [m](const T& x){
        std::lock_guard<std::mutex> lock(m->mtx_);
        m->last_ = std::move(x);
      },
      [](std::exception_ptr) {
      },
      [](){}
    });
  }

protected:

public:
  behavior(T&& initial_value) noexcept :
    m_(std::make_shared<member>(std::move(initial_value)))
  {
    initialize();
  }

  behavior(const T& initial_value) noexcept :
    m_(std::make_shared<member>(initial_value))
  {
    initialize();
  }

  virtual ~behavior() = default;

  virtual observable<T> as_observable() const noexcept override {
    auto src = subject<T>::as_observable();
    auto m = m_;
    auto base_completed = subject<T>::completed();
    auto base_error = subject<T>::error();
    return observable<>::create<T>([src, m, base_completed, base_error](subscriber<T> s){
      auto sctl = internal::stream_controller<T>(s);
      if(base_completed){
        if(base_error != nullptr){
          sctl.sink_error(base_error);
        }
        else{
          sctl.sink_completed_force();
        }
      }
      else{
        sctl.sink_next([m](){
          std::lock_guard<std::mutex> lock(m->mtx_);
          return m->last_;
        }());
        src.subscribe(sctl.template new_observer<T>(
          [sctl](auto, const T& x){
            sctl.sink_next(x);
          },
          [sctl](auto, std::exception_ptr err){
            sctl.sink_error(err);
          },
          [sctl](auto serial) {
            sctl.sink_completed(serial);
          }
        ));
      }
    });
  }
};

} /* namespace subjects */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_behavior__) */
