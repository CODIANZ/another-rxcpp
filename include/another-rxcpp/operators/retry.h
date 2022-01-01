#if !defined(__h_retry__)
#define __h_retry__

#include "../observable.h"

namespace another_rxcpp {

inline auto retry()
{
  return [](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src](subscriber<OUT> s) {
      auto proceed = std::make_shared<std::function<void()>>();
      *proceed = [src, s, proceed]() {
        auto upstream = src.create_source();
        upstream->subscribe({
          .on_next = [s](auto&& x){
            s.on_next(std::move(x));
          },
          .on_error = [proceed, upstream](std::exception_ptr){
            upstream->unsubscribe();
            (*proceed)();
          },
          .on_completed = [s](){
            s.on_completed();
          }
        });
      };
      (*proceed)();
    });
  };  
}

} /* namespace another_rxcpp */

#endif /* !defined(__h_retry__) */