#if !defined(__another_rxcpp_h_filter__)
#define __another_rxcpp_h_filter__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto filter(F f) noexcept
{
  return [f](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, f](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);

      source.subscribe(sctl.template new_observer<Item>(
        [sctl, f](auto, const Item& x){
          try{
            if(f(x)){
              sctl.sink_next(x);
            }
          }
          catch(...){
            sctl.sink_error(std::current_exception());
          }
        },
        [sctl](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl](auto serial) {
          sctl.sink_completed(serial);
        }
      ));
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_filter__) */