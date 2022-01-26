#if !defined(__another_rxcpp_h_on_error_resume_next__)
#define __another_rxcpp_h_on_error_resume_next__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

template <typename NEXT_FN> auto on_error_resume_next(NEXT_FN f)
{
  return [f](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        .on_next = [s](auto&& x){
          s.on_next(std::move(x));
        },
        .on_error = [s, f](std::exception_ptr err){
          try{
            f(err)
            .subscribe({
              .on_next = [s](auto&& x){
                s.on_next(std::move(x));
              },
              .on_error = [s](std::exception_ptr err){
                s.on_error(err);
              },
              .on_completed = [s](){
                s.on_completed();
              }
            });
          }
          catch(...){
            s.on_error(std::current_exception());
          }
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

#endif /* !defined(__another_rxcpp_h_on_error_resume_next__) */