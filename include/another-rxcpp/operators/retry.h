#if !defined(__another_rxcpp_h_retry__)
#define __another_rxcpp_h_retry__

#include "../observable.h"

namespace another_rxcpp {
namespace operators {

namespace internal::retry {

template <typename T>
  void do_retry(subscriber<T> s, observable<T> src, const std::size_t max_retry_count, const std::size_t count)
{
  auto upstream = src.create_source();
  s.add_upstream(upstream);
  upstream->subscribe({
    .on_next = [s](auto x){
      s.on_next(std::move(x));
    },
    .on_error = [s, src, upstream, max_retry_count, count](std::exception_ptr err){
      upstream->unsubscribe();
      if(max_retry_count == 0 || count < max_retry_count){
        do_retry(s, src, max_retry_count, count + 1);
      }
      else{
        s.on_error(err);
      }
    },
    .on_completed = [s](){
      s.on_completed();
    }
  });
}

} /* namespace internal::retry */

inline auto retry(std::size_t max_retry_count = 0 /* infinite */)
{
  return [max_retry_count](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, max_retry_count](subscriber<OUT> s) {
      internal::retry::do_retry<OUT>(s, src, max_retry_count, 0);
    });
  };  
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_retry__) */