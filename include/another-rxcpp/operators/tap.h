#if !defined(__another_rxcpp_h_tap__)
#define __another_rxcpp_h_tap__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename T>
  auto tap(observer<T> obs)
{
  return [obs](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, obs](subscriber<OUT> s) {
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, obs](auto x){
          if(obs.on_next) obs.on_next(x);
          s.on_next(std::move(x));
        },
        .on_error = [s, obs](std::exception_ptr err){
          if(obs.on_error) obs.on_error(err);
          s.on_error(err);
        },
        .on_completed = [s, obs](){
          if(obs.on_completed) obs.on_completed();
          s.on_completed();
        }
      });
    });
  };
}

template <typename NEXT>
  auto tap(
    NEXT  n,
    std::function<void(std::exception_ptr)> e = {},
    std::function<void()> c = {}
  )
{
  return [n, e, c](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n, e, c](subscriber<OUT> s) {
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, n](auto x){
          n(x);
          s.on_next(std::move(x));
        },
        .on_error = [s, e](std::exception_ptr err){
          if(e) e(err);
          s.on_error(err);
        },
        .on_completed = [s, c](){
          if(c) c();
          s.on_completed();
        }
      });
    });
  };
}





} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_tap__) */
