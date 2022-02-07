#if !defined(__another_rxcpp_h_take_until__)
#define __another_rxcpp_h_take_until__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename TRIGGER_OB> auto take_until(TRIGGER_OB trigger) noexcept
{
  return [trigger](auto src) {
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, trigger](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      auto trig = private_access::observable::create_source(trigger);
      private_access::subscriber::add_upstream(s, trig);
      trig->subscribe({
        [s](const auto&){
          s.on_completed();
        },
        [](std::exception_ptr){
        },
        [](){
        }
      });

      upstream->subscribe({
        [s](const auto& x){
          s.on_next(x);
        },
        [s, trig](std::exception_ptr err){
          s.on_error(err);
        },
        [s, trig](){
          s.on_completed();
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_take_until__) */