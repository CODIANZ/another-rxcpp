#if !defined(__h_subscribe_on__)
#define __h_subscribe_on__

#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto subscribe_on(scheduler::creator_fn sccr)
{
  return [sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, sccr](subscriber<OUT> s) {
      auto scdl = sccr();
      scdl.run([s, src]() mutable {
        auto upstream = src.create_source();
        upstream->subscribe({
          .on_next = [s](auto x){
            s.on_next(std::move(x));
          },
          .on_error = [s](std::exception_ptr err){
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

#endif /* !defined(__h_subscribe_on__) */