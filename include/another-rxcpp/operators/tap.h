#if !defined(__another_rxcpp_h_tap__)
#define __another_rxcpp_h_tap__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename T>
  auto tap(observer<T> obs) noexcept
{
  return [obs](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, obs](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        [s, obs](const auto& x){
          if(obs.on_next) obs.on_next(x);
          s.on_next(x);
        },
        [s, obs](std::exception_ptr err){
          if(obs.on_error) obs.on_error(err);
          s.on_error(err);
        },
        [s, obs](){
          if(obs.on_completed) obs.on_completed();
          s.on_completed();
        }
      });
    });
  };
}

template <typename ON_NEXT>
  auto tap(
    ON_NEXT                 n,
    observer<>::error_t     e = {},
    observer<>::completed_t c = {}
  ) noexcept
{
  return [n, e, c](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n, e, c](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        [s, n](const auto& x){
          n(x);
          s.on_next(x);
        },
        [s, e](std::exception_ptr err){
          if(e) e(err);
          s.on_error(err);
        },
        [s, c](){
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
