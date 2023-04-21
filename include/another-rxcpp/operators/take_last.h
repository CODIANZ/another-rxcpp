#if !defined(__another_rxcpp_h_take_last__)
#define __another_rxcpp_h_take_last__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"
#include <queue>
#include <mutex>

namespace another_rxcpp {
namespace operators {

inline auto take_last(std::size_t n) noexcept
{
  return [n](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, n](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      auto mtx = std::make_shared<std::mutex>();
      auto arr = std::make_shared<std::queue<Item>>();
      source.subscribe(sctl.template new_observer<Item>(
        [n, mtx, arr](auto, const Item& x) {
          std::lock_guard<std::mutex> lock(*mtx);
          arr->push(x);
          if(arr->size() > n) arr->pop();
        },
        [sctl](auto, std::exception_ptr err){
          sctl.sink_error(err);
        },
        [sctl, n, mtx, arr](auto serial){
          {
            /** Should I emit even with a small number of elements? */
            std::lock_guard<std::mutex> lock(*mtx);
            while(!arr->empty()) {
              sctl.sink_next(arr->front());
              arr->pop();
            }
          }
          sctl.sink_completed(serial);
        }
      ));
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_take_last__) */
