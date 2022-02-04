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
#include "../tools/util.h"
#include "source_base.h"

namespace another_rxcpp { namespace internal {

template <typename T = void> class source;

template <> class source<void> {
public:
  template <typename T, typename EMITTER_FN = typename source<T>::emitter_fn_t>
    static typename source<T>::sp create(const EMITTER_FN& emitter) noexcept
  {
    return std::make_shared<source<T>>(emitter);
  }

  template <typename T, typename EMITTER_FN = typename source<T>::emitter_fn_t>
    static typename source<T>::sp create(EMITTER_FN&& emitter) noexcept
  {
    return std::make_shared<source<T>>(std::move(emitter));
  }
};

template <typename T> class source : public source_base {
public:
  using value_type        = T;
  using sp                = std::shared_ptr<source<value_type>>;
  using observer_type     = observer<value_type>;
  using subscriber_type   = subscriber<value_type>;
  using subscriber_sp     = std::shared_ptr<subscriber_type>;
  using emitter_fn_t      = std::function<void(subscriber_type)>;

private:
  emitter_fn_t  emitter_fn_;
  subscriber_sp subscriber_;
  sp shared_this() noexcept { return std::dynamic_pointer_cast<source<T>>(shared_base()); }

protected:
  source() = default;

  virtual void unsubscribe_upstreams() noexcept override {
    auto s = subscriber_;
    subscriber_.reset();
    if(s){
      using namespace another_rxcpp::internal;
      private_access::subscriber::unsubscribe_upstreams(*s);
    }
  }

public:
  source(const emitter_fn_t& emitter_fn) noexcept : emitter_fn_(emitter_fn) {}
  source(emitter_fn_t&& emitter_fn) noexcept : emitter_fn_(std::move(emitter_fn)) {}
  virtual ~source() = default;

  subscription subscribe(const observer_type& ob) noexcept {
    auto cpob = ob;
    return subscribe(std::move(cpob));
  }

  virtual subscription subscribe(observer_type&& ob) noexcept {
    auto THIS = shared_this();
    auto obs = internal::to_shared(std::move(ob));
    subscriber_ = std::make_shared<subscriber_type>(THIS, obs);
    auto subscriber = subscriber_;
    subscription sbsc(
      any_sp_keeper::create(THIS, obs),
      /* is_subscribed() */
      [THIS]() {
        return THIS->state() == state::active;
      },
      /* on_unsubscribe */
      [THIS]() {
        THIS->set_state_unsubscribed();
        THIS->unsubscribe_upstreams();
      }
    );
    set_subscription(sbsc);
    try{
      emitter_fn_(*subscriber);
    }
    catch(...){
      subscriber->on_error(std::current_exception());
    }
    return sbsc;
  }
};

}} /* namespace another_rxcpp::internal */

#endif /* !defined(__another_rxcpp_h_source__) */
