#if !defined(__h_delay__)
#define __h_delay__

#include "../observable.h"
#include "../internal/tools/util.h"

#include <chrono>

namespace another_rxcpp {
namespace operators {

inline auto delay(std::chrono::milliseconds msec, scheduler::creator_fn sccr = schedulers::default_scheduler())
{
  /** TODO: implement scheduler */
  return [msec, sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, msec, sccr](subscriber<OUT> s) {
      auto scdl = sccr();
      scdl.schedule([src, msec, s](){
        auto upstream = src.create_source();
        s.add_upstream(upstream);
        upstream->subscribe({
          .on_next = [s, upstream, msec](auto x){
            std::this_thread::sleep_for(msec);
            s.on_next(std::move(x));
          },
          .on_error = [s, upstream](std::exception_ptr err){
            s.on_error(err);
          },
          .on_completed = [s](){
            s.on_completed();
          }
        });
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_delay__) */
