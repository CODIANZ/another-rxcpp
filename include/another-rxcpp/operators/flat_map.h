#if !defined(__another_rxcpp_h_flat_map__)
#define __another_rxcpp_h_flat_map__

#include "../observable.h"
#include "../internal/tools/util.h"
#include <atomic>
#include <type_traits>

namespace another_rxcpp {
namespace operators {

template <typename F> auto flat_map(F f)
{
  using OUT_OB  = lambda_invoke_result_t<F>;
  using OUT     = typename OUT_OB::value_type;
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto bUpstreamCompleted = std::make_shared<std::atomic_bool>(false);
      auto fxCounter = std::make_shared<std::atomic_int>(0);
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, f, upstream, bUpstreamCompleted, fxCounter](auto x){
          try{
            (*fxCounter)++;
            f(std::move(x))
            .subscribe({
              .on_next = [s](auto x){
                s.on_next(std::move(x));
              },
              .on_error = [s, upstream, fxCounter](std::exception_ptr err){
                (*fxCounter)--;
                s.on_error(err);
              },
              .on_completed = [s, bUpstreamCompleted, fxCounter](){
                (*fxCounter)--;
                if(*bUpstreamCompleted && (*fxCounter) == 0){
                  s.on_completed();
                }
              }
            });
          }
          catch(...){
            s.on_error(std::current_exception());
          }
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s, bUpstreamCompleted, fxCounter](){
          *bUpstreamCompleted = true;
          if(*bUpstreamCompleted && (*fxCounter) == 0){
            s.on_completed();
          }
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_flat_map__) */