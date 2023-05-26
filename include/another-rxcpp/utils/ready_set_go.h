#if !defined(__another_rxcpp_h_ready_set_go__)
#define __another_rxcpp_h_ready_set_go__

#include "../observable.h"
#include "../internal/tools/fn.h"
#include "../internal/tools/stream_controller.h"
#include <exception>

namespace another_rxcpp {
namespace utils {

template <typename OB>
  auto ready_set_go(internal::fn<void()> f, OB o) noexcept
{
  using T = typename OB::value_type;
  return observable<>::create<T>([f, o](subscriber<T> s){
    auto sctl = internal::stream_controller<T>(s);
    o.subscribe(sctl.template new_observer<T>(
      [sctl](auto, const T& x) {
        sctl.sink_next(x);
      },
      [sctl](auto, std::exception_ptr err){
        sctl.sink_error(err);
      },
      [sctl](auto serial){
        sctl.sink_completed(serial);
      }
    ));
    try{
      f();
    }
    catch(...){
      sctl.sink_error(std::current_exception());
    }
  });
}

} /* namespace utils */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_ready_set_go__) */