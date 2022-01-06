#if !defined(__h_interval__)
#define __h_interval__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename T = int>
  auto interval(std::chrono::milliseconds msec, scheduler::creator_fn sccr = schedulers::default_scheduler())
    -> observable<T>
{
  return observable<>::create<T>([msec, sccr](subscriber<T> s){
    auto scdl = sccr();
    scdl.schedule([s, msec](){
      T n = 0;
      while(true){
        std::this_thread::sleep_for(msec);
        if(!s.is_subscribed()){
          break;
        }
        s.on_next(n);
        n++;
      }
    });
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_interval__) */
