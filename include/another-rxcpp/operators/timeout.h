#if !defined(__h_timeout__)
#define __h_timeout__

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

inline auto timeout(std::chrono::milliseconds msec)
{
  return [msec](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observable<>::create<OUT>([src, msec](subscriber<OUT> s) {
      struct member {
        std::mutex              mtx_;
        std::thread             thread_;
        std::condition_variable cond_;
      };
      auto m = std::make_shared<member>();
      auto upstream = src.create_source();

      m->thread_ = std::thread([s, upstream, m, msec]{
        while(upstream->is_subscribed()){
          std::unique_lock<std::mutex> lock(m->mtx_);
          if(m->cond_.wait_for(lock, msec) == std::cv_status::timeout){
            if(upstream->is_subscribed()){
              upstream->unsubscribe();
              s.on_error(std::make_exception_ptr(timeout_error("timeout")));
            }
            break;
          }
        }
      });

      upstream->subscribe({
        .on_next = [s, upstream, m](auto x){
          m->cond_.notify_one();
          s.on_next(std::move(x));
        },
        .on_error = [s, upstream, m](std::exception_ptr err){
          s.on_error(err);
          m->cond_.notify_one();
          m->thread_.detach();
        },
        .on_completed = [s, m](){
          s.on_completed();
          m->cond_.notify_one();
          m->thread_.detach();
        }
      });
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_timeout__) */
