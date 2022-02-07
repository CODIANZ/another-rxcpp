#if !defined(__another_rxcpp_h_flat_map__)
#define __another_rxcpp_h_flat_map__

#include "../observable.h"
#include "../internal/tools/util.h"
#include <atomic>
#include <type_traits>

namespace another_rxcpp {
namespace operators {

template <typename F> auto flat_map(F f) noexcept
{
  using OUT_OB  = internal::lambda_invoke_result_t<F>;
  using OUT     = typename OUT_OB::value_type;
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto bUpstreamCompleted = std::make_shared<std::atomic_bool>(false);
      auto fxCounter = std::make_shared<std::atomic_int>(0);
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        [s, f, upstream, bUpstreamCompleted, fxCounter](const auto& x){
          try{
            (*fxCounter)++;
            auto fsrc = private_access::observable::create_source(f(x));
            private_access::subscriber::add_upstream(s, fsrc);
            fsrc->subscribe({
              [s](const auto& x){
                s.on_next(x);
              },
              [s, upstream, fxCounter](std::exception_ptr err){
                (*fxCounter)--;
                s.on_error(err);
              },
              [s, bUpstreamCompleted, fxCounter](){
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
        [s](std::exception_ptr err){
          s.on_error(err);
        },
        [s, bUpstreamCompleted, fxCounter](){
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