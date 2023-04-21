#if !defined(__another_rxcpp_h_subscribe_on__)
#define __another_rxcpp_h_subscribe_on__

#include "../observable.h"
#include "../scheduler.h"
#include "../internal/tools/stream_controller.h"

namespace another_rxcpp {
namespace operators {

inline auto subscribe_on(scheduler::creator_fn sccr) noexcept
{
  return [sccr](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, sccr](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto scdl = sccr();
      auto keepalive = scdl;
      scdl.schedule([sctl, source, keepalive]() {
        source.subscribe(sctl.template new_observer<Item>(
          [sctl, keepalive](auto, const Item& x) {
            sctl.sink_next(x);
          },
          [sctl, keepalive](auto, std::exception_ptr err){
            sctl.sink_error(err);
          },
          [sctl, keepalive](auto serial){
            sctl.sink_completed(serial);
          }
        ));
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscribe_on__) */