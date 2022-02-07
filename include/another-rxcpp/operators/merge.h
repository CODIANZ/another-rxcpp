#if !defined(__another_rxcpp_h_merge__)
#define __another_rxcpp_h_merge__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace merge_internal {
  template <typename T, typename OB>
  auto merge(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob) noexcept {
    arr.push_back(ob);
    return [sccr,  arr = internal::to_shared(std::move(arr))](auto src) {
      return observable<>::create<T>([src, sccr, arr](subscriber<T> s) {
        auto scdl = sccr();
        scdl.schedule([src, arr, s](){
          using namespace another_rxcpp::internal;
          using source_sp = typename OB::source_sp;
          std::vector<source_sp> sources;
          sources.push_back(private_access::observable::create_source(src));
          std::for_each(arr->begin(), arr->end(), [&](auto it){
            sources.push_back(private_access::observable::create_source(it));
          });

          std::for_each(sources.begin(), sources.end(), [&](auto it){
            private_access::subscriber::add_upstream(s, it);
          });

          auto completed = std::make_shared<std::atomic_size_t>(0);

          std::for_each(sources.begin(), sources.end(), [s, completed, sources](auto it){
            it->subscribe({
              [s](const auto& x){
                s.on_next(x);
              },
              [s](std::exception_ptr err){
                s.on_error(err);
              },
              [s, completed, sources](){
                (*completed)++;
                if(*completed == sources.size()){
                  s.on_completed();
                }
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