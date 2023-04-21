#if !defined(__another_rxcpp_h_tap__)
#define __another_rxcpp_h_tap__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"

namespace another_rxcpp {
namespace operators {

template <typename T>
  auto tap(observer<T> obs) noexcept
{
  return [obs](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, obs](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      source.subscribe(sctl.template new_observer<Item>(
        [sctl, obs](auto, const Item& x){
          obs.on_next(x);
          sctl.sink_next(x);
        },
        [sctl, obs](auto, std::exception_ptr err){
          obs.on_error(err);
          sctl.sink_error(err);
        },
        [sctl, obs](auto serial) {
          obs.on_completed();
          sctl.sink_completed(serial);
        }
      ));
    });
  };
}

template <typename ON_NEXT>
  auto tap(
    ON_NEXT                 n,
    observer<>::error_t     e = {},
    observer<>::completed_t c = {}
  ) noexcept
{
  return [n, e, c](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, n, e, c](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      source.subscribe(sctl.template new_observer<Item>(
        [sctl, n](auto, const Item& x){
          n(x);
          sctl.sink_next(x);
        },
        [sctl, e](auto, std::exception_ptr err){
          if(e) e(err);
          sctl.sink_error(err);
        },
        [sctl, c](auto serial) {
          if(c) c();
          sctl.sink_completed(serial);
        }
      ));
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_tap__) */
