#if !defined(__h_map__)
#define __h_map__

#include "../observable.h"

namespace another_rxcpp {

template <typename F> auto map(F f)
{
  using OUT = decltype(f({}));
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s, f, upstream](auto&& x){
          try{
            s.on_next(f(std::move(x)));
          }
          catch(...){
            upstream->unsubscribe();
            s.on_error(std::current_exception());
          }
        },
        .on_error = [s, upstream](std::exception_ptr err){
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

#endif /* !defined(__h_map__) */