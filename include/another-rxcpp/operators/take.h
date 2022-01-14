#if !defined(__another_rxcpp_h_take__)
#define __another_rxcpp_h_take__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

inline auto take(std::size_t n)
{
  return [n](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto counter = std::make_shared<std::atomic_size_t>(1);
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        .on_next = [s, n, upstream, counter](auto x){
          const auto now = counter->fetch_add(1);
          if(now == n) {
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

#endif /* !defined(__another_rxcpp_h_take__) */