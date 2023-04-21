#if !defined(__another_rxcpp_h_observable__)
#define __another_rxcpp_h_observable__

#include <functional>
#include <memory>
#include "observer.h"
#include "subscription.h"
#include "subscriber.h"
#include "internal/tools/fn.h"

namespace another_rxcpp {

template <typename T = void> class observable;

template <typename T> struct is_observable : std::false_type {};
template <typename T> struct is_observable<observable<T>> : std::true_type {};

template <> class observable<void> {
public:
  template<typename T>
    static auto create(const typename observable<T>::source_t& f) noexcept
  {
    return observable<T>(
      std::make_shared<typename observable<T>::source_t>(f)
    );    
  }

  template<typename T>
    static auto create(typename observable<T>::source_t&& f) noexcept
  {
    return observable<T>(
      std::make_shared<typename observable<T>::source_t>(std::move(f))
    );
  }


  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    #include "internal/supports/observable_static_decl.inc"
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */
};


template <typename T> class observable {
public:
  using value_type      = T;
  using observer_type   = observer<value_type>;
  using subscriber_type = subscriber<value_type>;
  using source_t        = internal::fn<void(subscriber_type)>;
  using source_sp       = std::shared_ptr<source_t>;
  using next_t          = typename observer_type::next_t;
  using error_t         = typename observer_type::error_t;
  using completed_t     = typename observer_type::completed_t;

private:
  source_sp source_;

protected:
  observable() = default;

public:
  observable(source_sp source) : source_(source) {}

  virtual subscription subscribe(observer_type ob) const noexcept {
    (*source_)(ob);
    return subscription(
      [ob] {
        ob.unsubscribe();
      },
      [ob] {
        return ob.is_subscribed();
      }
    );
  }

  subscription subscribe(
    const next_t&       n = {},
    const error_t&      e = {},
    const completed_t&  c = {}
  ) const noexcept {
    return subscribe(observer<value_type>(n, e, c));
  }

  subscription subscribe(
    next_t&&       n = {},
    error_t&&      e = {},
    completed_t&&  c = {}
  ) const noexcept {
    return subscribe(observer<value_type>(
      std::move(n),
      std::move(e),
      std::move(c)
    ));
  }


  template <typename F> auto operator | (F&& f) const noexcept
  {
    /**
     * F = auto f(observable<T>)
     * ex)
     *   auto map(std::function<T(T)>)
     *    -> std::function<observable<T>(observable<T>)>
     **/
    return f(*this);
  }

  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
    #include "internal/supports/operators_in_observables_decl.inc"
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    auto as_dynamic() const noexcept -> observable<value_type>
    {
      return *this;
    }
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */
};

} /* namespace another_rxcpp */

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include "internal/supports/observable_static_impl.inc"
#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */

#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
  #include "internal/supports/operators_in_observables_impl.inc"
#endif /* SUPPORTS_OPERATORS_IN_OBSERVABLE */

#endif /* !defined(__another_rxcpp_h_observable__) */
