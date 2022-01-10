#if !defined(__h_finally__)
#define __h_finally__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto finally(F f)
{
  return [f](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, upstream](auto x){
          s.on_next(std::move(x));
        },
        .on_error = [s, upstream, f](std::exception_ptr err){
          s.on_error(err);
          f();
        },
        .on_completed = [s, f](){
          s.on_completed();
          f();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_finally__) */
