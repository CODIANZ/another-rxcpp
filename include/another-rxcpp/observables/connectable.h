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
  template <typename T> static auto create(observable<T> src) noexcept {
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

  struct member {
    sinks_type      sinks_;
    std::mutex      mtx_;
    std::atomic_int serial_{0};
    source_sp       upstream_;
  };
  std::shared_ptr<member> m_;

  class connectable_source : public internal::source<value_type> {
  private:
    std::shared_ptr<member> m_;

  public:
    connectable_source(std::shared_ptr<member> m) noexcept : m_(m) {}

    virtual subscription subscribe(observer_type&& ob) noexcept override {
      const auto serial = m_->serial_.fetch_add(1);
      auto m = m_;
      subscription sbsc(
        internal::any_sp_keeper::create(),
        /* is_subscribed() */
        [serial, m]() {
          std::lock_guard<std::mutex> lock(m->mtx_);
          auto it = m->sinks_.find(serial);
          return it != m->sinks_.end() && m->upstream_->is_subscribed();
        },
        /* on_unsubscribe */
        [serial, m]() {
          std::lock_guard<std::mutex> lock(m->mtx_);
          m->sinks_.erase(serial);
          if(m->sinks_.size() == 0) m->upstream_->unsubscribe();
        }
      );
      {
        std::lock_guard<std::mutex> lock(m->mtx_);
        m->sinks_.insert({serial, {
          .subscription_ = sbsc,
          .observer_ = std::move(ob)
        }});
      }
      return sbsc;
    }
  };

public:
  connectable() = default; /* do not use this normally! */
  connectable(observable<T> src) noexcept : 
    m_(std::make_shared<member>())
  {
    m_->upstream_ = internal::private_access::observable::create_source(src);
    auto m = m_;
    observable<value_type>::set_source_creator_fn(
      [m](){
        return std::dynamic_pointer_cast<source_type>(
          std::make_shared<connectable_source>(m)
        );
      }
    );
  }

  subscription connect() const noexcept {
    auto m = m_;

    auto collect = [m](){
      std::lock_guard<std::mutex> lock(m->mtx_);
      std::vector<observer_type> ret(m->sinks_.size());
      auto ret_it = ret.begin();
      for(auto it = m->sinks_.begin(); it != m->sinks_.end(); it++, ret_it++){
        *ret_it = it->second.observer_;
      }
      return ret;
    };

    m->upstream_->subscribe({
      [collect](const value_type& x) {
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_next(x);
        });
      },
      [collect](std::exception_ptr err){
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_error(err);
        });
      },
      [collect](){
        auto obs = collect();
        std::for_each(obs.begin(), obs.end(), [&](auto ob){
          ob.on_completed();
        });
      }
    });

    return subscription(
      internal::any_sp_keeper::create(),
      /* is_subscribed() */
      [m](){
        return m->upstream_->is_subscribed();
      },
      /* on_unsubscribe */
      [m](){
        m->upstream_->unsubscribe();
        std::lock_guard<std::mutex> lock(m->mtx_);
        m->sinks_.clear();
      }
    );
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_connectable_observable__) */
