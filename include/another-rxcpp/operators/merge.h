#if !defined(__another_rxcpp_h_merge__)
#define __another_rxcpp_h_merge__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"
#include "../schedulers/default_scheduler.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace merge_internal {
  template <typename T, typename OB>
  auto merge(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob) noexcept {
    arr.push_back(ob);
    return [sccr,  arr = internal::to_shared(std::move(arr))](auto src) {
      arr->push_back(src);
      return observable<>::create<T>([sccr, arr](subscriber<T> s) {
        auto sctl = internal::stream_controller<T>(s);
        auto scdl = sccr();

        // prepare subscribers
        auto subscribers = [sctl, scdl, arr]{
          auto re = std::vector<subscriber<T>>();
          for(auto i = 0; i < arr->size(); i++){
            re.push_back(
              sctl.template new_observer<T>(
                [sctl, scdl](auto, const T& x) {
                  scdl.schedule([sctl, x]{
                    sctl.sink_next(x);
                  });
                },
                [sctl, scdl](auto, std::exception_ptr err){
                  scdl.schedule([sctl, err]{
                    sctl.sink_error(err);
                  });
                },
                [sctl, scdl](auto serial){
                  scdl.schedule([sctl, serial]{
                    sctl.sink_completed(serial);
                  });
                }
              )
            );
          }
          return re;
        }();

        for(auto i = 0; i < arr->size(); i++){
          (*arr)[i].subscribe(subscribers[i]);
        }
      });
    };  
  }

  template <typename T, typename OB, typename...ARGS>
  auto merge(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob, ARGS...args){
    arr.push_back(ob);
    return merge(sccr, arr, args...);
  }
} /* merge_internal */

template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool> = true>
auto merge(OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return merge_internal::merge<T>(schedulers::default_scheduler(), arr, ob, args...);
}

template <typename OB, typename...ARGS>
auto merge(scheduler::creator_fn sccr, OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return merge_internal::merge<T>(sccr, arr, ob, args...);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_merge__) */