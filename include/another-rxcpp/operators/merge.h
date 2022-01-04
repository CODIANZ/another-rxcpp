#if !defined(__h_merge__)
#define __h_merge__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace internal {
  template <typename T, typename OB>
  auto merge(schedulers::scheduler scl, std::vector<observable<T>>& arr, OB ob){
    arr.push_back(ob);
    return [scl, arr](auto src) mutable {
      return observable<>::create<T>([src, scl, arr](subscriber<T> s) mutable {
        scl.run([src, scl, arr, s](){
          using source_sp = typename OB::source_sp;
          std::vector<source_sp> sources;
          sources.push_back(src.create_source());
          std::for_each(arr.begin(), arr.end(), [&](auto it){
            sources.push_back(it.create_source());
          });

          auto unsubscribe_all = [sources](){
            std::for_each(sources.begin(), sources.end(), [](auto it){
              it->unsubscribe();
            });
          };

          std::for_each(sources.begin(), sources.end(), [s, unsubscribe_all](auto it){
            it->subscribe({
              .on_next = [s](auto&& x){
                s.on_next(std::move(x));
              },
              .on_error = [s, unsubscribe_all](std::exception_ptr err){
                unsubscribe_all();
                s.on_error(err);
              },
              .on_completed = [s, unsubscribe_all](){
                unsubscribe_all();
                s.on_completed();
              }
            });
          });
        });
      });
    };  
  }

  template <typename T, typename OB, typename...ARGS>
  auto merge(schedulers::scheduler scl, std::vector<observable<T>>& arr, OB ob, ARGS...args){
    arr.push_back(ob);
    return merge(scl, arr, args...);
  }
} /* namespace internal */

template <typename OB, typename...ARGS>
auto merge(OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::merge<T>(schedulers::default_scheduler(), arr, ob, args...);
}

template <typename OB, typename...ARGS>
auto merge(schedulers::scheduler scl, OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::merge<T>(scl, arr, ob, args...);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_merge__) */