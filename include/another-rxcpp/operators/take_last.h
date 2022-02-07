#if !defined(__another_rxcpp_h_take_last__)
#define __another_rxcpp_h_take_last__

#include "../observable.h"
#include "../internal/tools/util.h"
#include <queue>

namespace another_rxcpp {
namespace operators {

inline auto take_last(std::size_t n) noexcept
{
  return [n](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, n](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      auto mtx = std::make_shared<std::mutex>();
      auto arr = std::make_shared<std::queue<OUT>>();
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);
      upstream->subscribe({
        [s, upstream, n, mtx, arr](const auto& x){
          std::lock_guard<std::mutex> lock(*mtx);
          arr->push(x);
          if(arr->size() > n) arr->pop();
        },
        [s, upstream](std::exception_ptr err){
          s.on_error(err);
        },
        [s, n, mtx, arr](){
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

#endif /* !defined(__another_rxcpp_h_take_last__) */
