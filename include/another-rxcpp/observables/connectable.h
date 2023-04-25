#if !defined(__another_rxcpp_h_connectable_observable__)
#define __another_rxcpp_h_connectable_observable__

#include "../observable.h"
#include <unordered_map>
#include <mutex>
#include <vector>

namespace another_rxcpp {
namespace observables {

template <typename T = void> class connectable;

template <> class connectable<void> {
public:
  template <typename T> static auto create(observable<T> src) noexcept {
    return connectable<T>(src);
  }
};

template <typename T> class connectable : public observable<T> {
friend class connectable<>;
public:
  using value_type        = typename observable<T>::value_type;
  using source_observable = observable<value_type>;
  using observer_type     = typename observable<T>::observer_type;
  using serial_type       = int32_t;
  using observer_map      = std::unordered_map<serial_type, observer_type>;

private:
  struct member {
    source_observable     source_;
    observer_map          observers_;
    std::recursive_mutex  mtx_;
    serial_type           serial_;
    subscription          subscription_;
    member(source_observable&& source): source_(std::move(source)) {}
    member(const source_observable& source): source_(source) {}
  };
  std::shared_ptr<member> m_;

public:
  connectable() = default; /* do not use this normally! */
  connectable(const source_observable& source) noexcept : 
    m_(std::make_shared<member>(source))
    {}
  connectable(source_observable&& source) noexcept : 
    m_(std::make_shared<member>(std::move(source)))
    {}


  virtual subscription subscribe(observer_type ob) const noexcept override {
    auto m = m_;

    const auto serial = [&]{
      std::lock_guard<std::recursive_mutex> lock(m->mtx_);
      return m->serial_++;
    }();

    m->observers_.insert({serial, ob});

    return subscription(
      [m, serial]{
        std::lock_guard<std::recursive_mutex> lock(m->mtx_);
        auto it = m->observers_.find(serial);
        if(it != m->observers_.end()){
          m->observers_.erase(it);
          // TODO: What should I do?
          // enable below if not forever
          // if(m->observers_.size() == 0){
          //   m->subscription_.unsubscribe();
          // }
        }
      },
      [m, serial]{
        std::lock_guard<std::recursive_mutex> lock(m->mtx_);
        auto it = m->observers_.find(serial);
        if(it != m->observers_.end()){
          return it->second.is_subscribed();
        }
        else{
          return false;
        }
      }
    );
  }

  subscription connect() const noexcept {
    auto m = m_;

    auto collect = [m](bool clear){
      std::lock_guard<std::recursive_mutex> lock(m->mtx_);
      std::vector<observer_type> ret(m->observers_.size());
      auto ret_it = ret.begin();
      for(auto it = m->observers_.begin(); it != m->observers_.end(); it++, ret_it++){
        *ret_it = it->second;
      }
      if(clear) m->observers_.clear();
      return ret;
    };

    auto sbsc = m->source_.subscribe(
      [collect, m](value_type x) {
        auto obs = collect(false);
        std::for_each(obs.begin(), obs.end(), [m, &x](auto ob){
          ob.on_next(x);
        });
      },
      [collect, m](std::exception_ptr err){
        auto obs = collect(true);
        std::for_each(obs.begin(), obs.end(), [m, &err](auto ob){
          ob.on_error(err);
        });
      },
      [collect, m](){
        auto obs = collect(true);
        std::for_each(obs.begin(), obs.end(), [m](auto ob){
          ob.on_completed();
        });
      }
    );

    std::lock_guard<std::recursive_mutex> lock(m->mtx_);
    auto sbsc2 = subscription(
      [sbsc, m]{
        sbsc.unsubscribe();
        std::lock_guard<std::recursive_mutex> lock(m->mtx_);
        m->observers_.clear();
        m->subscription_ = subscription();
      },
      [sbsc]{
        return sbsc.is_subscribed();
      }
    );
    m->subscription_ = sbsc2;
    return sbsc2;
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_connectable_observable__) */
