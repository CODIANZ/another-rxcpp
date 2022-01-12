#if !defined(__another_rxcpp_h_ready_set_go__)
#define __another_rxcpp_h_ready_set_go__

#include "../observable.h"

namespace another_rxcpp {
namespace utils {

template <typename OB>
  auto ready_set_go(std::function<void()> f, OB o)
{
  using T = typename OB::value_type;
  return observable<>::create<T>([f, o](subscriber<T> s){
    o.subscribe({
      .on_next = [s](T x) {
        s.on_next(std::move(x));
      },
      .on_error = [s](std::exception_ptr err){
        s.on_error(err);
      },
      .on_completed = [s](){
        s.on_completed();
      }
    });
    f();
  });
}

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_ready_set_go__) */