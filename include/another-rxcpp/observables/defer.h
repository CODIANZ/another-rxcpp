#if !defined(__another_rxcpp_h_defer__)
#define __another_rxcpp_h_defer__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"

namespace another_rxcpp {
namespace observables {

template <typename F>
  inline auto defer(F&& fn, scheduler::creator_fn sccr = schedulers::default_scheduler()) noexcept
    -> decltype(fn())
{
  using OB = decltype(fn());
  using T = typename OB::value_type;
  return observable<>::create<T>([fn, sccr](subscriber<T> s) {
    auto scdl = sccr();
    scdl.schedule([fn, s]() {
      using namespace another_rxcpp::internal;
      const auto o = fn();
      const auto src = private_access::observable::create_source(o);
      private_access::subscriber::add_upstream(s, src);
      src->subscribe({
        [s, src](const auto& x) {
          s.on_next(x);
        },
        [s, src](std::exception_ptr err) {
          s.on_error(err);
        },
        [s, src](){
          s.on_completed();
        }
      });
    });
  });
}

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_defer__) */