#if !defined(__h_map__)
#define __h_map__

#include "../observable.h"

namespace another_rxcpp {

template <typename F> auto map(F f)
{
  using OUT = decltype(f({}));
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto src_ = src;
      src_.subscribe({
        .on_next = [s, f](auto&& x){
          s.on_next(f(std::move(x)));
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
          s.on_completed();
        }
      });
    });
  };
}

} /* namespace another_rxcpp */

#endif /* !defined(__h_map__) */