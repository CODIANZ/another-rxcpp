#if !defined(__h_take__)
#define __h_take__

#include "../observable.h"

namespace another_rxcpp {

inline auto take(std::size_t n)
{
  return [n](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n](subscriber<OUT> s) {
      auto counter = std::make_shared<std::size_t>(0);
      auto mtx = std::make_shared<std::mutex>();
      auto src_ = src;
      src_.subscribe({
        .on_next = [s, n, counter, mtx](auto&& x){
          const bool bNext = [&](){
            std::lock_guard<std::mutex> lock(*mtx);
            return *counter < n;
          }();
          if(bNext) s.on_next(std::move(x));

          const bool bComplete = [&](){
            std::lock_guard<std::mutex> lock(*mtx);
            if(*counter == n){
              return true;
            }
            else{
              (*counter)++;
              return *counter == n;
            }
          }();
          if(bComplete) s.on_completed();
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

} /* namespace another_rxcpp */

#endif /* !defined(__h_take__) */