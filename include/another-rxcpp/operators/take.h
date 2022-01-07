#if !defined(__h_take__)
#define __h_take__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

inline auto take(std::size_t n)
{
  return [n](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n](subscriber<OUT> s) {
      auto counter = std::make_shared<std::atomic_size_t>(1);
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, n, upstream, counter](auto x){
          const auto now = counter->fetch_add(1);
          if(now == n) {
            upstream->unsubscribe();
            s.on_next(std::move(x));
            s.on_completed();
          }
          else if(now < n){
            s.on_next(std::move(x));
          }
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

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_take__) */