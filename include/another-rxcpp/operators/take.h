#if !defined(__another_rxcpp_h_take__)
#define __another_rxcpp_h_take__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"
#include <atomic>
#include <condition_variable>

namespace another_rxcpp {
namespace operators {

inline auto take(std::size_t n) noexcept
{
  return [n](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, n](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto counter = std::make_shared<std::atomic_size_t>(1);

      source.subscribe(sctl.template new_observer<Item>(
        [sctl, n, counter](auto serial, const Item& x) {
          const auto now = counter->fetch_add(1);
          if(now == n) {
            sctl.upstream_abort_observe(serial);
            sctl.sink_next(x);
            sctl.sink_completed(serial);
          }
          else if(now < n){
            sctl.sink_next(x);
          }
        },
        [sctl](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl](auto serial){
          sctl.sink_completed(serial);
        }
      ));
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_take__) */