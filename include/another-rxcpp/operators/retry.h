#if !defined(__another_rxcpp_h_retry__)
#define __another_rxcpp_h_retry__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

namespace retry_internal {

template <typename Item>
  void do_retry(
    internal::stream_controller<Item> sctl,
    observable<Item> source,
    const std::size_t max_retry_count,
    const std::size_t count
  ) noexcept
{

  source.subscribe(sctl.template new_observer<Item>(
    [sctl](auto, const Item& x){
      sctl.sink_next(x);
    },
    [sctl, source, max_retry_count, count](auto serial, std::exception_ptr err){
      sctl.upstream_abort_observe(serial);
      if(max_retry_count == 0 || count < max_retry_count){
        do_retry(sctl, source, max_retry_count, count + 1);
      }
      else{
        sctl.sink_error(err);
      }
    },
    [sctl](auto serial){
     sctl.sink_completed(serial);
    }
  ));
}

} /* namespace retry_internal */

inline auto retry(std::size_t max_retry_count = 0 /* infinite */)
{
  return [max_retry_count](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;

    return observable<>::create<Item>([source, max_retry_count](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      retry_internal::do_retry<Item>(sctl, source, max_retry_count, 0);
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_retry__) */