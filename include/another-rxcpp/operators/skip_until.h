#if !defined(__another_rxcpp_h_skip_until__)
#define __another_rxcpp_h_skip_until__

#include "../observable.h"
#include "../internal/tools/stream_controller.h"

namespace another_rxcpp {
namespace operators {

template <typename TRIGGER_OB> auto skip_until(TRIGGER_OB trigger) noexcept
{
  return [trigger](auto source) {
    using Source = decltype(source);
    using Item = typename Source::value_type;
    using TriggerItem = typename TRIGGER_OB::value_type;
    return observable<>::create<Item>([source, trigger](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto enable = std::make_shared<std::atomic_bool>(false);

      auto obs_trigger = sctl.template new_observer<TriggerItem>(
        [sctl, enable](auto serial, auto) {
          sctl.upstream_abort_observe(serial);
          (*enable) = true;
        },
        [sctl](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl](auto serial){
          sctl.sink_completed(serial);
        }
      );

      auto obs_source = sctl.template new_observer<Item>(
        [sctl, enable](auto, const Item& x) {
          if(*enable){
            sctl.sink_next(x);
          }
        },
        [sctl](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl](auto serial){
          sctl.sink_completed_force(); // trigger also unsubscribe
        }
      );

      trigger.subscribe(obs_trigger);
      source.subscribe(obs_source);
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_skip_until__) */