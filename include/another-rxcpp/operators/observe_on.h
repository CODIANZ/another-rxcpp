#if !defined(__h_observe_on__)
#define __h_observe_on__

#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto observe_on(scheduler scdl)
{
  return [scdl](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, scdl](subscriber<OUT> s) {
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, scdl](auto&& x){
          scdl.run([s, x, scdl /* keep-alive */]() mutable {
            s.on_next(std::move(x));
          });
        },
        .on_error = [s, scdl /* keep-alive */](std::exception_ptr err){
          scdl.run([s, err, scdl /* keep-alive */]() mutable {
            s.on_error(err);
          });
        },
        .on_completed = [s, scdl /* keep-alive */](){
          scdl.run([s, scdl /* keep-alive */]() mutable {
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