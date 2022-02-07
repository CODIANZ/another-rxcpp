#if !defined(__another_rxcpp_h_skip_while__)
#define __another_rxcpp_h_skip_while__

#include "../observable.h"
#include "../internal/tools/util.h"

namespace another_rxcpp {
namespace operators {

template <typename F> auto skip_while(F f) noexcept
{
  return [f](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, f](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      auto skip = std::make_shared<bool>(true);
      upstream->subscribe({
        [s, f, skip](const auto& x){
          try{
            if(*skip){
              *skip = f(x);
            }
            if(!*skip){
              s.on_next(x);
            }
          }
          catch(...){
            s.on_error(std::current_exception());
          }
        },
        [s](std::exception_ptr err){
          s.on_error(err);
        },
        [s](){
          s.on_completed();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_skip_while__) */