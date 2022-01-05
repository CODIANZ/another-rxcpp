#if !defined(__h_amb__)
#define __h_amb__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace internal {
  template <typename T, typename OB>
  auto amb(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob){
    arr.push_back(ob);
    return [sccr, arr](auto src) mutable {
      return observable<>::create<T>([src, sccr, arr](subscriber<T> s) mutable {
        auto scdl = sccr();
        scdl.run([src, arr, s](){
          using source_sp = typename OB::source_sp;
          using source_type = typename OB::source_type;

          auto mtx = std::make_shared<std::mutex>();
          auto top = std::make_shared<source_type*>();
          std::vector<source_sp> sources;

          sources.push_back(src.create_source());
          std::for_each(arr.begin(), arr.end(), [&](auto it){
            sources.push_back(it.create_source());
          });

          auto do_on_next = [sources, s, mtx, top](source_sp sp, auto value){
            {
              std::lock_guard<std::mutex> lock(*mtx);
              if(*top == nullptr){
                *top = sp.get();
                std::for_each(sources.begin(), sources.end(), [sp](auto it){
                  if(sp.get() != it.get()){
                    it->unsubscribe();
                  }
                });
              }
            }
            if(*top && *top == sp.get()){
              s.on_next(std::move(value));
            }
          };

          std::for_each(sources.begin(), sources.end(), [s, do_on_next](auto it){
            it->subscribe({
              .on_next = [it, s, do_on_next](auto&& x){
                do_on_next(it, std::move(x));
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
  auto amb(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob, ARGS...args){
    arr.push_back(ob);
    return amb(sccr, arr, args...);
  }
} /* namespace internal */

template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool> = true>
auto amb(OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::amb<T>(schedulers::default_scheduler(), arr, ob, args...);
}

template <typename OB, typename...ARGS>
auto amb(scheduler::creator_fn sccr, OB ob, ARGS...args) {
  using T = typename OB::value_type;
  std::vector<observable<T>> arr;
  return internal::amb<T>(sccr, arr, ob, args...);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_amb__) */