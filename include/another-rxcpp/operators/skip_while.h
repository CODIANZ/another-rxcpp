#if !defined(__another_rxcpp_h_skip_while__)
#define __another_rxcpp_h_skip_while__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto skip_while(F f) noexcept
{
  return [f](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, f](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto skip = std::make_shared<bool>(true);
      source.subscribe(sctl.template new_observer<Item>(
        [sctl, f, skip](auto, const Item& x){
          try{
            if(*skip){
              *skip = f(x);
            }
            if(!*skip){
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
        [sctl](auto serial){
          sctl.sink_completed(serial);
        }
      ));
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_skip_while__) */