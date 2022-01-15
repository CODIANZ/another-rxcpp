#if !defined(__another_rxcpp_h_connectable_observable__)
#define __another_rxcpp_h_connectable_observable__

#include "../observable.h"
#include <algorithm>
#include <unordered_map>
#include <atomic>

namespace another_rxcpp {
namespace observables {

template <typename T = void> class connectable;

template <> class connectable<void> {
public:
  template <typename T> static auto create(observable<T> src) {
    return connectable<T>(src);
  }
};

template <typename T> class connectable : public observable<T> {
friend class connectable<>;
public:
  using value_type    = typename observable<T>::value_type;
  using source_type   = typename observable<T>::source_type;
  using source_sp     = typename observable<T>::source_sp;
  using observer_type = typename observable<T>::observer_type;
  using source_creator_fn_t = typename observable<T>::source_creator_fn_t;

private:
  struct sink {
    subscription  subscription_;
    observer_type observer_;
  };

  using sinks_type      = std::unordered_map<int, sink>;
  using sinks_sp        = std::shared_ptr<sinks_type>;
  using mutex_sp        = std::shared_ptr<std::mutex>;
  using serrial_sp      = std::shared_ptr<std::atomic_int>;
  using subscription_sp = std::shared_ptr<subscription>;

  sinks_sp        sinks_;
  source_sp       upstream_;
  mutex_sp        mtx_;
  serrial_sp      serial_;

public:
  connectable() = default; /* do not use this normally! */
  connectable(observable<T> src) : 
    mtx_(std::make_shared<std::mutex>()),
    serial_(std::make_shared<std::atomic_int>(0)),
    sinks_(std::make_shared<sinks_type>()),
    upstream_(internal::private_access::observable::create_source(src))
  {}

  subscription connect() const {
    auto mtx    = mtx_;
    auto sinks  = sinks_;

    auto collect = [mtx, sinks](){
      std::lock_guard<std::mutex> lock(*mtx);
      std::vector<observer_type> ret(sinks->size());
      auto ret_it = ret.begin();
      for(auto it = sinks->begin(); it != sinks->end(); it++, ret_it++){
        *ret_it = it->second.observer_;
      }
      return ret;
    };

    upstream_->subscribe({
      .on_next = [collect](value_type x) {
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

    auto ups = upstream_;
    return subscription(
      internal::any_sp_keeper::create(),
      /* is_subscribed() */
      [ups](){
        return ups->is_subscribed();
      },
      /* on_unsubscribe */
      [mtx, sinks, ups](){
        ups->unsubscribe();
        std::lock_guard<std::mutex> lock(*mtx);
        sinks->clear();
      }
    );
  }

 virtual subscription subscribe(observer_type ob) const override {
    const auto serial = serial_->fetch_add(1);
    auto mtx    = mtx_;
    auto sinks  = sinks_;
    auto ups    = upstream_;
    subscription sbsc(
      internal::any_sp_keeper::create(),
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
    return sbsc;
  }

  /* TODO: Find out if this is really needed. */
  subscription subscribe(
    typename observer_type::on_next_fn_t      on_next = {},
    typename observer_type::on_error_fn_t     on_error = {},
    typename observer_type::on_completed_fn_t on_completed = {}
  ) const {
    return subscribe({
      .on_next      = on_next,
      .on_error     = on_error,
      .on_completed = on_completed
    });
  }

  template <typename F> auto operator | (F f) const
  {
    /**
     * !! CAUTION !!
     * This definition is required. 
     * See comments for the same function in the superclass.
     **/
    return f(*this);
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_connectable_observable__) */
