#if !defined(__h_observe_on__)
#define __h_observe_on__

#include "../observable.h"
#include "../schedulers/scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto observe_on(schedulers::scheduler scdl)
{
  return [scdl](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, scdl](subscriber<OUT> s) {
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, scdl](auto&& x){
          scdl.run([s, x]() mutable {
            s.on_next(std::move(x));
          });
        },
        .on_error = [s, scdl](std::exception_ptr err){
          scdl.run([s, err]() mutable {
            s.on_error(err);
          });
        },
        .on_completed = [s, scdl](){
          scdl.run([s]() mutable {
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