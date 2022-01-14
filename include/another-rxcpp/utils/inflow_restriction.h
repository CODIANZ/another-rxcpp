#if !defined(__another_rxcpp_h_inflow_restriction__)
#define __another_rxcpp_h_inflow_restriction__

#include "../observable.h"
#include "sem.h"

namespace another_rxcpp {
namespace utils {

template <int N>
class inflow_restriction {
private:
  using sem_type = utils::sem<N>;
  std::shared_ptr<sem_type> sem_;

public:
  inflow_restriction() :
    sem_(std::make_shared<sem_type>()) {}
  inflow_restriction(const inflow_restriction&) = delete;
  ~inflow_restriction() = default;

  template <typename T>
    auto enter(observable<T> o)
  {
    auto sem = sem_;
    return observable<>::create<T>([sem, o](subscriber<T> s){
      sem->lock();
      o.subscribe({
        .on_next = [s](T v){
          s.on_next(std::move(v));
        },
        .on_error = [s, sem](std::exception_ptr e){
            s.on_error(e);
          sem->unlock();
        },
        .on_completed = [s, sem](){
          s.on_completed();
          sem->unlock();
        }
      });
    });
  }
};

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_ready_set_go__) */