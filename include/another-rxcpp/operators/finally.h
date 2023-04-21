#if !defined(__another_rxcpp_h_finally__)
#define __another_rxcpp_h_finally__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto finally(F f) noexcept
{
  return [f](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, f](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      source.subscribe(sctl.template new_observer<Item>(
        [sctl](auto, const Item& x){
          sctl.sink_next(x);
        },
        [sctl, f](auto, std::exception_ptr err){
          sctl.sink_error(err);
          f();
        },
        [sctl, f](auto serial) {
          sctl.sink_completed(serial);
          f();
        }
      ));
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_finally__) */
