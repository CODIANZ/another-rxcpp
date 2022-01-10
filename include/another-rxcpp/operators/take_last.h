#if !defined(__h_take_last__)
#define __h_take_last__

#include "../observable.h"
#include "../internal/tools/util.h"
#include <queue>

namespace another_rxcpp {
namespace operators {

inline auto take_last(std::size_t n)
{
  return [n](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n](subscriber<OUT> s) {
      auto mtx = std::make_shared<std::mutex>();
      auto arr = std::make_shared<std::queue<OUT>>();
      auto upstream = src.create_source();
      s.add_upstream(upstream);
      upstream->subscribe({
        .on_next = [s, upstream, n, mtx, arr](auto x){
          std::lock_guard<std::mutex> lock(*mtx);
          arr->push(std::move(x));
          if(arr->size() > n) arr->pop();
        },
        .on_error = [s, upstream](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s, n, mtx, arr](){
          {
            /** Should I emit even with a small number of elements? */
            std::lock_guard<std::mutex> lock(*mtx);
            while(!arr->empty()) {
              s.on_next(arr->front());
              arr->pop();
            }
          }
          s.on_completed();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_take_last__) */
