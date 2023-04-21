#if !defined(__another_rxcpp_h_delay__)
#define __another_rxcpp_h_delay__

#include "../observable.h"
#include "../scheduler.h"
#include "../schedulers/default_scheduler.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"

#include <thread>
#include <chrono>

namespace another_rxcpp {
namespace operators {

inline auto delay(std::chrono::milliseconds msec, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
{
  return [msec, sccr](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, msec, sccr](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto scdl = sccr();
      scdl.schedule([source, msec, sctl](){
        source.subscribe(sctl.template new_observer<Item>(
          [sctl, msec](auto, const Item& x){
            std::this_thread::sleep_for(msec);
            sctl.sink_next(x);
          },
          [sctl](auto, std::exception_ptr err){
            sctl.sink_error(err);
          },
          [sctl](auto serial) {
            sctl.sink_completed(serial);
          }
        ));
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_delay__) */
