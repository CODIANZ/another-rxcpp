#if !defined(__another_rxcpp_h_subscribe_on__)
#define __another_rxcpp_h_subscribe_on__

#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto subscribe_on(scheduler::creator_fn sccr) noexcept
{
  return [sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, sccr](subscriber<OUT> s) {
      auto scdl = sccr();
      auto keepalive = scdl;
      scdl.schedule([s, src, keepalive]() {
        using namespace another_rxcpp::internal;
        auto upstream = private_access::observable::create_source(src);
        private_access::subscriber::add_upstream(s, upstream);
        upstream->subscribe({
          [s, keepalive](const auto& x){
            s.on_next(x);
          },
          [s, keepalive](std::exception_ptr err){
            s.on_error(err);
          },
          [s, keepalive](){
            s.on_completed();
          }
        });
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscribe_on__) */