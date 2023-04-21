#if !defined(__another_rxcpp_h_observe_on__)
#define __another_rxcpp_h_observe_on__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto observe_on(scheduler::creator_fn sccr) noexcept
{
  return [sccr](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, sccr](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto scdl = sccr();
      source.subscribe(sctl.template new_observer<Item>(
        [sctl, scdl](auto, const Item& x) {
          scdl.schedule([sctl, x]() mutable {
            sctl.sink_next(std::move(x));
          });
        },
        [sctl, scdl](auto, std::exception_ptr err){
          scdl.schedule([sctl, err]() {
            sctl.sink_error(err);
          });
        },
        [sctl, scdl](auto serial){
          scdl.schedule([sctl, serial]() {
            sctl.sink_completed(serial);
          });
        }
      ));
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_observe_on__) */