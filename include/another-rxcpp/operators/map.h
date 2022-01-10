#if !defined(__h_map__)
#define __h_map__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto map(F f)
{
  using OUT = lambda_invoke_result_t<F>;
  return [f](auto src){
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, f](auto x){
          try{
            s.on_next(f(std::move(x)));
          }
          catch(...){
            s.on_error(std::current_exception());
          }
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

#endif /* !defined(__h_map__) */