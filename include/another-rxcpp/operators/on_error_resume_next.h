#if !defined(__h_on_error_resume_next__)
#define __h_on_error_resume_next__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename NEXT_FN> auto on_error_resume_next(NEXT_FN f)
{
  return [f](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      auto bUpstreamCompleted = std::make_shared<std::atomic_bool>(false);
      auto fxCounter = std::make_shared<std::atomic_int>(0);
      auto upstream = src.create_source();
      upstream->subscribe({
        .on_next = [s](auto x){
          s.on_next(std::move(x));
        },
        .on_error = [s, f, upstream, bUpstreamCompleted, fxCounter](std::exception_ptr err){
          try{
            (*fxCounter)++;
            f(err)
            .subscribe({
              .on_next = [s](auto x){
                s.on_next(std::move(x));
              },
              .on_error = [s, upstream, fxCounter](std::exception_ptr err){
                upstream->unsubscribe();
                (*fxCounter)--;
                s.on_error(err);
              },
              .on_completed = [s, bUpstreamCompleted, fxCounter](){
                (*fxCounter)--;
                if(*bUpstreamCompleted && (*fxCounter) == 0){
                  s.on_completed();
                }
              }
            });
          }
          catch(...){
            upstream->unsubscribe();
            s.on_error(std::current_exception());
          }
        },
        .on_completed = [s, bUpstreamCompleted, fxCounter](){
          *bUpstreamCompleted = true;
          if(*bUpstreamCompleted && (*fxCounter) == 0){
            s.on_completed();
          }
        }
      });
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_on_error_resume_next__) */