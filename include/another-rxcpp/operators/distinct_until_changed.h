#if !defined(__another_rxcpp_h_distinct_until_changed__)
#define __another_rxcpp_h_distinct_until_changed__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

inline auto distinct_until_changed()
{
  return [](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src](subscriber<OUT> s) {
      auto mtx = std::make_shared<std::mutex>();
      auto last_value = std::make_shared<std::shared_ptr<OUT>>();
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, mtx, last_value](auto x){
          const bool bNext = [&](){
            std::lock_guard<std::mutex> lock(*mtx);
            if(!*last_value){
              *last_value = std::make_shared<OUT>(std::move(x));
              return true;
            }
            if(**last_value != x){
              **last_value = std::move(x);
              return true;
            }
            return false;
          }();
          if(bNext) s.on_next(**last_value);
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
          s.on_completed();
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_distinct_until_changed__) */