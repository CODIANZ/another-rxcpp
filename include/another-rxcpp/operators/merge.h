#if !defined(__another_rxcpp_h_merge__)
#define __another_rxcpp_h_merge__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace internal {
  template <typename T, typename OB>
  auto merge(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob){
    arr.push_back(ob);
    return [sccr, arr](auto src) mutable {
      return observable<>::create<T>([src, sccr, arr](subscriber<T> s) mutable {
        auto scdl = sccr();
        scdl.schedule([src, arr, s](){
          using source_sp = typename OB::source_sp;
          std::vector<source_sp> sources;
          sources.push_back(src.create_source());
          std::for_each(arr.begin(), arr.end(), [&](auto it){
            sources.push_back(it.create_source());
          });

          std::for_each(sources.begin(), sources.end(), [&](auto it){
            s.add_upstream(it);
          });

          std::for_each(sources.begin(), sources.end(), [s](auto it){
            it->subscribe({
              .on_next = [s](auto x){
                s.on_next(std::move(x));
              },
              .on_error = [s](std::exception_ptr err){
                s.on_error(err);
              },
              .on_completed = [s](){
                s.on_completed();
              }
            });
          });
        });
      });
    };  
  }

  template <typename T, typename OB, typename...ARGS>
  auto merge(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob, ARGS...args){
    arr.push_back(ob);
    return merge(sccr, arr, args...);
  }
} /* namespace internal */

template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool> = true>
auto merge(OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::merge<T>(schedulers::default_scheduler(), arr, ob, args...);
}

template <typename OB, typename...ARGS>
auto merge(scheduler::creator_fn sccr, OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::merge<T>(sccr, arr, ob, args...);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_merge__) */