#if !defined(__another_rxcpp_h_observe_on__)
#define __another_rxcpp_h_observe_on__

#include "../observable.h"
#include "../scheduler.h"

namespace another_rxcpp {
namespace operators {

inline auto observe_on(scheduler::creator_fn sccr) noexcept
{
  return [sccr](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, sccr](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto scdl = sccr();
      auto upstream =private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        [s, scdl, upstream](const auto& x){
          scdl.schedule([s, x]() {
            s.on_next(std::move(x));
          });
        },
        [s, scdl](std::exception_ptr err){
          scdl.schedule([s, err]() {
            s.on_error(err);
          });
        },
        [s, scdl](){
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

#endif /* !defined(__another_rxcpp_h_observe_on__) */