#if !defined(__another_rxcpp_h_source__)
#define __another_rxcpp_h_source__

#include <memory>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <exception>

#include "../../subscriber.h"
#include "../../observer.h"
#include "../../subscription.h"
#include "source_base.h"
#include "../tools/any_sp_keeper.h"

namespace another_rxcpp {

template <typename T = void> class source;

template <> class source<void> {
public:
  template <typename T>
  static typename source<T>::sp create(
    typename source<T>::creator_fn_t creator
  ) {
    return std::make_shared<source<T>>([creator](auto, auto s){
      creator(s);
    });
  }

  template <typename T, typename UPSTREAM_SP>
  static typename source<T>::sp create(
    UPSTREAM_SP upstream,
    typename source<T>::emitter_fn_t emitter
  ) {
    return std::make_shared<source<T>>(
      std::dynamic_pointer_cast<source_base>(upstream),
      emitter
    );
  }
};

template <typename T> class source : public source_base {
public:
  using value_type        = T;
  using sp                = std::shared_ptr<source<value_type>>;
  using observer_type     = observer<value_type>;
  using subscriber_type   = subscriber<value_type>;
  using creator_fn_t      = std::function<void(subscriber_type)>;
  using emitter_fn_t      = std::function<void(sp, subscriber_type)>;

private:
  emitter_fn_t  emitter_fn_;

  sp shared_this() { return std::dynamic_pointer_cast<source<T>>(shared_base()); }

protected:
  source() = default;
  emitter_fn_t emitter() { return emitter_fn_; }

public:
  source(emitter_fn_t emitter_fn) : emitter_fn_(emitter_fn) {}
  virtual ~source() = default;

  virtual subscription subscribe(observer_type ob) {
    auto THIS = shared_this();
    auto obs = ob.to_shared();
    std::weak_ptr<source<value_type>> WEAK_THIS = shared_this();
    subscriber_type subscriber(THIS, obs);
    subscription sbsc(
      any_sp_keeper::create(THIS, obs),
      /* is_subscribed() */
      [WEAK_THIS]() {
        auto p = WEAK_THIS.lock();
        if(!p) return false;
        return p->state() == state::active;
      },
      /* on_unsubscribe */
      [THIS]() {
        THIS->set_state_unsubscribed();
      }
    );
    set_subscription(sbsc);
    try{
      emitter_fn_(THIS, subscriber);
    }
    catch(...){
      subscriber.on_error(std::current_exception());
    }
    return sbsc;
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_source__) */
