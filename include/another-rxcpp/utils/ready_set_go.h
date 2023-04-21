#if !defined(__another_rxcpp_h_ready_set_go__)
#define __another_rxcpp_h_ready_set_go__

#include "../observable.h"
#include "../internal/tools/fn.h"

namespace another_rxcpp {
namespace utils {

template <typename OB>
  auto ready_set_go(internal::fn<void()> f, OB o) noexcept
{
  using T = typename OB::value_type;
  return observable<>::create<T>([f, o](subscriber<T> s){
    o.subscribe({
      [s](const T& x) {
        s.on_next(x);
      },
      [s](std::exception_ptr err){
        s.on_error(err);
      },
      [s](){
        s.on_completed();
      }
    });
    f();
  });
}

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_ready_set_go__) */