#if !defined(__another_rxcpp_h_on_error_resume_next__)
#define __another_rxcpp_h_on_error_resume_next__

#include "../internal/tools/stream_controller.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename NEXT_FN> auto on_error_resume_next(NEXT_FN f) noexcept
{
  return [f](auto source){
    using Source = decltype(source);
    using Item = typename Source::value_type;
    return observable<>::create<Item>([source, f](subscriber<Item> s) {
      auto sctl = internal::stream_controller<Item>(s);
      
      source.subscribe(sctl.template new_observer<Item>(
        [sctl](auto, const Item& x) {
          sctl.sink_next(x);
        },
        [sctl, f](auto serial, std::exception_ptr err){
          sctl.upstream_abort_observe(serial);
          try{
            f(err).subscribe(sctl.template new_observer<Item>(
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
          }
          catch(...){
            sctl.sink_error(std::current_exception());
          }
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

#endif /* !defined(__another_rxcpp_h_on_error_resume_next__) */