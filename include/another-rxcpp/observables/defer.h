#if !defined(__another_rxcpp_h_defer__)
#define __another_rxcpp_h_defer__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"
#include "../schedulers/default_scheduler.h"

namespace another_rxcpp {
namespace observables {

template <typename F>
  inline auto defer(F&& fn, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
    -> decltype(fn())
{
  using Source = decltype(fn());
  using Item = typename Source::value_type;
  return observable<>::create<Item>([fn, sccr](subscriber<Item> s) {
    auto sctl = internal::stream_controller<Item>(s);
    auto scdl = sccr();
    sctl.set_on_finalize([scdl]{
      scdl.abort();
    });
    scdl.schedule([fn, sctl]() {
      fn().subscribe(sctl.template new_observer<Item>(
        [sctl](auto, const Item& x){
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
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_defer__) */