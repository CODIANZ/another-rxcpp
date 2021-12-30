#if !defined(__h_flat_map__)
#define __h_flat_map__

#include "../observable.h"
#include <atomic>

namespace another_rxcpp {

template <typename F> auto flat_map(F f)
{
  using OUT_OB = decltype(f({}));
  using OUT = typename OUT_OB::value_type;
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto bUpstreamCompleted = std::make_shared<std::atomic_bool>(false);
      auto fxCounter = std::make_shared<std::atomic_int>(0);
      auto src_ = src;
      src_.subscribe({
        .on_next = [s, f, bUpstreamCompleted, fxCounter](auto&& x){
          (*fxCounter)++;
          f(std::move(x))
          .subscribe({
            .on_next = [s](auto&& x){
              s.on_next(std::move(x));
            },
            .on_error = [s, fxCounter](std::exception_ptr err){
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

} /* namespace another_rxcpp */

#endif /* !defined(__h_flat_map__) */