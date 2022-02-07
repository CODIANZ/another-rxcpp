#if !defined(__another_rxcpp_h_delay__)
#define __another_rxcpp_h_delay__

#include "../observable.h"
#include "../internal/tools/util.h"

#include <chrono>

namespace another_rxcpp {
namespace operators {

inline auto delay(std::chrono::milliseconds msec, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
{
  return [msec, sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, msec, sccr](subscriber<OUT> s) {
      auto scdl = sccr();
      scdl.schedule([src, msec, s](){
        using namespace another_rxcpp::internal;
        auto upstream = private_access::observable::create_source(src);
        private_access::subscriber::add_upstream(s, upstream);
        upstream->subscribe({
          [s, upstream, msec](const auto& x){
            std::this_thread::sleep_for(msec);
            s.on_next(x);
          },
          [s, upstream](std::exception_ptr err){
            s.on_error(err);
          },
          [s](){
            s.on_completed();
          }
        });
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_delay__) */
