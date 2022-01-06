#if !defined(__h_take_until__)
#define __h_take_until__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename TRIGGER_OB> auto take_until(TRIGGER_OB trigger)
{
  return [trigger](auto src) mutable {
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, trigger](subscriber<OUT> s) mutable {
      auto upstream = src.create_source();
      auto trig = trigger.create_source();
      trig->subscribe({
        .on_next = [upstream, s, trig](auto){
          trig->unsubscribe();
          upstream->unsubscribe();
          s.on_completed();
        },
        .on_error = [](std::exception_ptr){
        },
        .on_completed = [](){
        }
      });

      upstream->subscribe({
        .on_next = [s](auto x){
          s.on_next(std::move(x));
        },
        .on_error = [s, trig](std::exception_ptr err){
          trig->unsubscribe();
          s.on_error(err);
        },
        .on_completed = [s, trig](){
          trig->unsubscribe();
          s.on_completed();
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_take_until__) */