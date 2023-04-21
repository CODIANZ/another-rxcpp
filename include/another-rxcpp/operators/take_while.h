#if !defined(__another_rxcpp_h_take_while__)
#define __another_rxcpp_h_take_while__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto take_while(F f) noexcept
{
  return [f](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, f](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      source.subscribe(sctl.template new_observer<Item>(
        [sctl, f](auto serial, const Item& x){
          try{
            if(f(x)){
              sctl.sink_next(x);
            }
            else{
              sctl.sink_completed(serial);
            }
          }
          catch(...){
            sctl.sink_error(std::current_exception());
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

#endif /* !defined(__another_rxcpp_h_take_while__) */