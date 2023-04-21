#if !defined(__another_rxcpp_h_distinct_until_changed__)
#define __another_rxcpp_h_distinct_until_changed__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

inline auto distinct_until_changed() noexcept
{
  return [](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);

      auto mtx = std::make_shared<std::recursive_mutex>();
      auto last_value = std::make_shared<std::shared_ptr<Item>>();

      source.subscribe(sctl.template new_observer<Item>(
        [sctl, mtx, last_value](auto, const Item& x){
          const bool bNext = [&](){
            std::lock_guard<std::recursive_mutex> lock(*mtx);
            if(!*last_value){
              *last_value = std::make_shared<Item>(x);
              return true;
            }
            if(**last_value != x){
              **last_value = x;
              return true;
            }
            return false;
          }();
          if(bNext){
            sctl.sink_next(x);
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

#endif /* !defined(__another_rxcpp_h_distinct_until_changed__) */