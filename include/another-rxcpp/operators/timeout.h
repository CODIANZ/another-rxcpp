#if !defined(__another_rxcpp_h_timeout__)
#define __another_rxcpp_h_timeout__

#include "../observable.h"
#include "../internal/tools/util.h"

#include <chrono>
#include <exception>

namespace another_rxcpp {
namespace operators {

class timeout_error : public std::runtime_error {
public:
  timeout_error(const std::string& msg) : runtime_error(msg) {}
};

inline auto timeout(std::chrono::milliseconds msec) noexcept
{
  return [msec](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, msec](subscriber<OUT> s) {
      using namespace another_rxcpp::internal;
      struct member {
        std::mutex              mtx_;
        std::condition_variable cond_;
      };
      auto m = std::make_shared<member>();
      auto upstream = private_access::observable::create_source(src);
      private_access::subscriber::add_upstream(s, upstream);

      std::thread([s, upstream, m, msec]{
        std::unique_lock<std::mutex> lock(m->mtx_);
        while(upstream->is_subscribed()){
          if(m->cond_.wait_for(lock, msec) == std::cv_status::timeout){
            if(upstream->is_subscribed()){
              s.on_error(std::make_exception_ptr(timeout_error("timeout")));
            }
            break;
          }
        }
      }).detach();

      upstream->subscribe({
        [s, m](const auto& x){
          m->cond_.notify_one();
          s.on_next(x);
        },
        [s, m](std::exception_ptr err){
          s.on_error(err);
          m->cond_.notify_one();
        },
        [s, m](){
          s.on_completed();
          m->cond_.notify_one();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_timeout__) */
