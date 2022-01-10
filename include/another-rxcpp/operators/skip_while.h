#if !defined(__h_skip_while__)
#define __h_skip_while__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto skip_while(F f)
{
  return [f](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto upstream = src.create_source();
      auto skip = std::make_shared<bool>(true);
      upstream->subscribe({
        .on_next = [s, f, skip, upstream](auto x){
          try{
            if(*skip){
              *skip = f(x);
            }
            if(!*skip){
              s.on_next(std::move(x));
            }
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

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_skip_while__) */