#if !defined(__another_rxcpp_h_map__)
#define __another_rxcpp_h_map__

#include "../internal/tools/stream_controller.h"
#include "../internal/tools/util.h"
#include "../observable.h"

namespace another_rxcpp { namespace operators {

template <typename F> auto map(F f) noexcept
{
  using Out = internal::lambda_invoke_result_t<F>;

  return [f](auto source){
    using Source = decltype(source);
    using In = typename Source::value_type;
    return observable<>::create<Out>([source, f](auto s){
      auto sctl = internal::stream_controller<Out>(s);
      
      source.subscribe(sctl.template new_observer<In>(
        [sctl, f](auto, const In& x) {
          try{
            sctl.sink_next(f(x));
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

}} /* namespace another_rxcpp::operators */
#endif /* !defined(__another_rxcpp_h_map__) */
