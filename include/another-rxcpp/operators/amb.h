#if !defined(__another_rxcpp_h_amb__)
#define __another_rxcpp_h_amb__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace amb_internal {
  template <typename T, typename OB>
  auto amb(scheduler::creator_fn sccr, std::vector<observable<T>>& arr, OB ob) noexcept {
    arr.push_back(ob);
    return [sccr, arr = internal::to_shared(std::move(arr))](auto src) {
      return observable<>::create<T>([src, sccr, arr](subscriber<T> s) {
        auto scdl = sccr();
        scdl.schedule([src, arr, s](){
          using namespace another_rxcpp::internal;
          using source_sp = typename OB::source_sp;
          using source_type = typename OB::source_type;

          auto mtx = std::make_shared<std::mutex>();
          auto top = std::make_shared<source_type*>();
          std::vector<source_sp> sources;

          auto upstream = private_access::observable::create_source(src);
          sources.push_back(private_access::observable::create_source(src));
          std::for_each(arr->begin(), arr->end(), [&](auto it){
            sources.push_back(private_access::observable::create_source(it));
          });

          std::for_each(sources.begin(), sources.end(), [&](auto it){
            private_access::subscriber::add_upstream(s, it);
          });

          auto do_on_next = [sources, s, mtx, top](source_sp sp, const auto& value){
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
              s.on_next(value);
            }
          };

          std::for_each(sources.begin(), sources.end(), [s, do_on_next](auto it){
            it->subscribe({
              [it, s, do_on_next](const auto& x){
                do_on_next(it, x);
              },
              [s](std::exception_ptr err){
                s.on_error(err);
              },
              [s](){
                s.on_completed();
              }
            });
          });
        });
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