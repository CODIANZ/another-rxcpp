#if !defined(__another_rxcpp_h_inflow_restriction__)
#define __another_rxcpp_h_inflow_restriction__

#include "../observable.h"
#include "../internal/tools/stream_controller.h"
#include "sem.h"

namespace another_rxcpp {
namespace utils {

template <int N>
class inflow_restriction {
private:
  using sem_type = utils::sem<N>;
  std::shared_ptr<sem_type> sem_;

public:
  inflow_restriction() noexcept :
    sem_(std::make_shared<sem_type>()) {}
  inflow_restriction(const inflow_restriction&) = delete;
  ~inflow_restriction() = default;

  template <typename T>
    auto enter(observable<T> o) noexcept
  {
    auto sem = sem_;
    return observable<>::create<T>([sem, o](subscriber<T> s){
      auto sctl = internal::stream_controller<T>(s);
      sctl.set_on_finalize([sem](){
        sem->unlock();
      });
      sem->lock();
      if(!sctl.is_subscribed()) {
        sem->unlock();
        return;
      }
      o.subscribe(sctl.template new_observer<T>(
        [sctl](auto, const T& v){
          sctl.sink_next(v);
        },
        [sctl, sem](auto, std::exception_ptr e){
          sctl.sink_error(e);
          sem->unlock();
        },
        [sctl, sem](auto serial){
          sctl.sink_completed(serial);
          sem->unlock();
        }
      ));
    });
  }
};

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_ready_set_go__) */