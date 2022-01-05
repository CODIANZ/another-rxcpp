#if !defined(__h_delay__)
#define __h_delay__

#include "../observable.h"
#include "../internal/tools/util.h"

#include <chrono>

namespace another_rxcpp {
namespace operators {

inline auto delay(std::chrono::milliseconds msec)
{
  /** TODO: not implemented */
  return [](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src](subscriber<OUT> s) {
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, upstream](auto&& x){
          s.on_next(std::move(x));
        },
        .on_error = [s, upstream](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
          s.on_completed();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_delay__) */
