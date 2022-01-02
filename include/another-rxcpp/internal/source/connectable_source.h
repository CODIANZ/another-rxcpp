#if !defined(__h_connectable_source__)
#define __h_connectable_source__

#include <memory>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <exception>
#include <algorithm>

#include "../../subscriber.h"
#include "../../observer.h"
#include "../../subscription.h"
#include "source_base.h"
#include "../tools/any_sp_keeper.h"

namespace another_rxcpp {

template <typename T> class connectable_source : public source<T> {
private:
  using value_type    = T;
  using souce_type    = source<value_type>;
  using source_sp     = typename souce_type::sp;
  using observer_type = typename souce_type::observer_type;
  using source_creator_fn_t = std::function<source_sp()>;

  struct sink {
    subscription  subscription_;
    observer_type observer_;
  };

  using sinks_type  = std::unordered_map<int, sink>;
  using sinks_sp    = std::shared_ptr<sinks_type>;
  using mutex_sp    = std::shared_ptr<std::mutex>;
  using serrial_sp  = std::shared_ptr<std::atomic_int>;

  sinks_sp      sinks_;
  source_sp     upstream_;
  mutex_sp      mtx_;
  serrial_sp    serial_;
  source_creator_fn_t upstream_creator_fn;

public:
  connectable_source(source_sp upstream) : upstream_(upstream)
  {
    mtx_    = std::make_shared<std::mutex>();
    serial_ = std::make_shared<std::atomic_int>(0);
    sinks_  = std::make_shared<sinks_type>();

    auto mtx    = mtx_;
    auto sinks  = sinks_;

    auto collect = [mtx, sinks](){
      std::lock_guard<std::mutex> lock(*mtx);
      std::vector<observer_type> ret(sinks->size());
      auto ret_it = ret.begin();
      for(auto it = sinks->begin(); it != sinks->end(); it++, ret_it++){
        *ret_it = it->second.observer_;
      }
      return std::move(ret);
    };

    upstream_->subscribe({
      .on_next = [collect](value_type&& x) {
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_next(x);
        });
      },
      .on_error = [collect](std::exception_ptr err){
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_error(err);
        });
      },
      .on_completed = [collect](){
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_completed();
        });
      }
    });
  }

  virtual ~connectable_source() = default;

  virtual subscription subscribe(observer_type ob) override {
    const auto serial = serial_->fetch_add(1);
    auto mtx    = mtx_;
    auto sinks  = sinks_;
    auto ups    = upstream_;
    subscription sbsc(
      any_sp_keeper::create(),
      /* is_subscribed() */
      [serial, mtx, sinks, ups]() {
        std::lock_guard<std::mutex> lock(*mtx);
        auto it = sinks->find(serial);
        return it != sinks->end() && ups->is_subscribed();
      },
      /* on_unsubscribe */
      [serial, mtx, sinks, ups]() mutable{
        std::lock_guard<std::mutex> lock(*mtx);
        sinks->erase(serial);
        if(sinks->size() == 0) ups->unsubscribe();
      }
    );
    {
      std::lock_guard<std::mutex> lock(*mtx);
      sinks->insert({serial, {
        .subscription_ = sbsc,
        .observer_ = ob
      }});
    }
    return std::move(sbsc);
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_connectable_source__) */
