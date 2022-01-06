#if !defined(__h_observe_on__)
#define __h_observe_on__

#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto observe_on(scheduler::creator_fn sccr)
{
  return [sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, sccr](subscriber<OUT> s) {
      auto scdl = sccr();
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, scdl](auto x){
          scdl.schedule([s, x]() {
            s.on_next(std::move(x));
          });
        },
        .on_error = [s, scdl](std::exception_ptr err){
          scdl.schedule([s, err]() {
            s.on_error(err);
          });
        },
        .on_completed = [s, scdl](){
          scdl.schedule([s]() {
            s.on_completed();
          });
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_observe_on__) */