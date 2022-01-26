#if !defined(__another_rxcpp_h_take_until__)
#define __another_rxcpp_h_take_until__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename TRIGGER_OB> auto take_until(TRIGGER_OB trigger)
{
  return [trigger](auto src) mutable {
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, trigger](subscriber<OUT> s) mutable {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      auto trig = private_access::observable::create_source(trigger);
      private_access::subscriber::add_upstream(s, trig);
      trig->subscribe({
        .on_next = [s](auto&&){
          s.on_completed();
        },
        .on_error = [](std::exception_ptr){
        },
        .on_completed = [](){
        }
      });

      upstream->subscribe({
        .on_next = [s](auto&& x){
          s.on_next(std::move(x));
        },
        .on_error = [s, trig](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s, trig](){
          s.on_completed();
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_take_until__) */