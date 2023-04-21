#if !defined(__another_rxcpp_h_flat_map__)
#define __another_rxcpp_h_flat_map__

#include "../internal/tools/stream_controller.h"
#include "../internal/tools/util.h"
#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto flat_map(F f) noexcept
{
  using OutObs  = internal::lambda_invoke_result_t<F>;
  using Out     = typename OutObs::value_type;
  return [f](auto source){
    return observable<>::create<Out>([source, f](auto s) {
      using Source = decltype(source);
      using In = typename Source::value_type;

      auto sctl = internal::stream_controller<Out>(s);
      
      source.subscribe(sctl.template new_observer<In>(
        [sctl, f](auto, const In& x) {
          try{
            f(x).subscribe(sctl.template new_observer<Out>(
              [sctl](auto, const Out& x){
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

#endif /* !defined(__another_rxcpp_h_flat_map__) */