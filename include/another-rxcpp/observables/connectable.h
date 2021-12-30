#if !defined(__h_connectable_observable__)
#define __h_connectable_observable__

#include "../observable.h"
#include <unordered_map>
#include <algorithm>

namespace another_rxcpp {
namespace observables {

template <typename T = void> class connectable;

template <> class connectable<void> {
public:
  template <typename T> static auto create(observable<T> src) {
    connectable<T> c{};
    c.start(src);
    return std::move(c);
  }
};

template <typename T> class connectable : public observable<T> {
friend class connectable<>;
public:
  using value_type    = T;
  using souce_type    = source<value_type>;
  using source_sp     = typename souce_type::sp;
  using observer_type = typename souce_type::observer_type;

private:

  struct sink {
    subscription  subscription;
    observer_type observer;
  };

  using sinks_type  = std::unordered_map<int, sink>;
  using sinks_sp    = std::shared_ptr<sinks_type>;
  using mutex_sp    = std::shared_ptr<std::mutex>;
  using serrial_sp  = std::shared_ptr<std::atomic_int>;

  sinks_sp      sinks_;
  subscription  upstream_subscription_;
  mutex_sp      mtx_;
  serrial_sp    serial_;

  void start(observable<value_type> ob){
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
        *ret_it = it->second.observer;
      }
      return std::move(ret);
    };

    upstream_subscription_ = ob.subscribe({
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

  connectable() {}

public: 
  virtual ~connectable() = default;

  virtual subscription subscribe(observer_type ob) override {
    const auto serial = serial_->fetch_add(1);
    auto mtx    = mtx_;
    auto sinks  = sinks_;
    auto ups    = upstream_subscription_;
    subscription sbsc([serial, mtx, sinks, ups]() mutable{
      std::lock_guard<std::mutex> lock(*mtx);
      sinks->erase(serial);
      if(sinks->size() == 0) ups.unsubscribe();
    }, [serial, mtx, sinks, ups]() {
      std::lock_guard<std::mutex> lock(*mtx);
      auto it = sinks->find(serial);
      return it != sinks->end() && ups.is_subscribed();
    });
    {
      std::lock_guard<std::mutex> lock(*mtx);
      sinks->insert({serial, {
        .subscription = sbsc,
        .observer = ob
      }});
    }
    return std::move(sbsc);
  }
};

}
} /* namespace another_rxcpp */

#endif /* !defined(__h_connectable_observable__) */
