#if !defined(__another_rxcpp_h_amb__)
#define __another_rxcpp_h_amb__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/stream_controller.h"
#include "../schedulers/default_scheduler.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace amb_internal {
  template <typename T, typename OB>
  auto amb(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob) noexcept {
    arr.push_back(ob);
    return [sccr, arr = internal::to_shared(std::move(arr))](auto src) {
      arr->push_back(src);
      return observable<>::create<T>([sccr, arr](subscriber<T> s) {
        auto sctl = internal::stream_controller<T>(s);
        using serial_type = typename internal::stream_controller<T>::serial_type;
        auto mtx = std::make_shared<std::recursive_mutex>();
        auto  winner = std::make_shared<std::shared_ptr<serial_type>>();
        auto is_win_next = [winner, mtx](serial_type serial) {
          std::lock_guard<std::recursive_mutex> lock(*mtx);
          if(*winner) {
            return **winner == serial;
          }
          *winner = std::make_shared<serial_type>(serial);
          return true;
        };
        auto scdl = sccr();

        // prepare subscribers
        auto subscribers = [sctl, scdl, arr, is_win_next] {
          auto re = std::vector<subscriber<T>>();
          for(auto i = 0; i < arr->size(); i++){
            re.push_back(
              sctl.template new_observer<T>(
                [sctl, scdl, is_win_next](auto serial, const T& x) {
                  if(is_win_next(serial)) {
                    scdl.schedule([sctl, x]{
                      sctl.sink_next(x);
                    });
                  }
                  else{
                    sctl.upstream_abort_observe(serial);
                  }
                },
                [sctl, scdl, is_win_next](auto serial, std::exception_ptr err) {
                  if(is_win_next(serial)) {
                    scdl.schedule([sctl, err]{
                      sctl.sink_error(err);
                    });
                  }
                  else{
                    sctl.upstream_abort_observe(serial);
                  }
                },
                [sctl, scdl, is_win_next](auto serial) {
                  if(is_win_next(serial)) {
                    scdl.schedule([sctl, serial]{
                      sctl.sink_completed(serial);
                    });
                  }
                  else{
                    sctl.upstream_abort_observe(serial);
                  }
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
  auto amb(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob, ARGS...args) noexcept {
    arr.push_back(ob);
    return amb(sccr, arr, args...);
  }
} /* namespace amb_internal */

template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool> = true>
auto amb(OB ob, ARGS...args) noexcept {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return amb_internal::amb<T>(schedulers::default_scheduler(), arr, ob, args...);
}

template <typename OB, typename...ARGS>
auto amb(scheduler::creator_fn sccr, OB ob, ARGS...args) noexcept {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return amb_internal::amb<T>(sccr, arr, ob, args...);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_amb__) */