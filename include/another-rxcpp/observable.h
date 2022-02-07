#if !defined(__another_rxcpp_h_observable__)
#define __another_rxcpp_h_observable__

#include "internal/source/source.h"
#include "schedulers.h"
#include "internal/tools/util.h"
#include "internal/tools/private_access.h"

namespace another_rxcpp {

template <typename T = void> class observable;

template <typename T> struct is_observable : std::false_type {};
template <typename T> struct is_observable<observable<T>> : std::true_type {};

template <> class observable<void> {
public:
  template<typename T, typename EMITTER_FN = typename internal::source<T>::emitter_fn_t>
    static auto create(const EMITTER_FN& f) noexcept
  {
    return observable<T>([f]() {
      return internal::source<>::create<T>(f);
    });
  }

  template<typename T, typename EMITTER_FN = typename internal::source<T>::emitter_fn_t>
    static auto create(EMITTER_FN&& f) noexcept
  {
    return observable<T>([f = std::move(f)]() {
      return internal::source<>::create<T>(f);
    });
  }

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)
    #include "internal/supports/observable_static_decl.inc"
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */
};

class observable_base {
protected:
  template <typename T> static typename internal::source<T>::sp execute_source_creator(const observable<T>& o) noexcept {
    return o.source_creator_fn_();
  }
public:
  observable_base() = default;
  virtual ~observable_base() = default;
};

template <typename T> class observable : public observable_base {
friend class observable_base;
friend class internal::private_access::observable;
public:
  using value_type    = T;
  using source_type   = internal::source<value_type>;
  using source_sp     = typename source_type::sp;
  using observer_type = typename source_type::observer_type;
  using source_creator_fn_t = std::function<source_sp()>;

private:
  source_creator_fn_t source_creator_fn_;

protected:
  observable() = default;
  source_sp create_source() const noexcept { return source_creator_fn_(); }
  void set_source_creator_fn(source_creator_fn_t source_creator) noexcept {
    source_creator_fn_ = source_creator;
  }

public: 
  observable(const source_creator_fn_t& source_creator) noexcept :
    source_creator_fn_(source_creator) {}
  observable(source_creator_fn_t&& source_creator) noexcept :
    source_creator_fn_(std::move(source_creator)) {}
  virtual ~observable() = default;

  subscription subscribe(const observer_type& ob) const noexcept {
    auto cpob = ob;
    return subscribe(std::move(cpob));
  }

  virtual subscription subscribe(observer_type&& ob) const noexcept {
    return source_creator_fn_()->subscribe(std::move(ob));
  }

  subscription subscribe(
    typename observer_type::on_next_fn_t      on_next = {},
    typename observer_type::on_error_fn_t     on_error = {},
    typename observer_type::on_completed_fn_t on_completed = {}
  ) const noexcept {
    return subscribe({
      on_next,
      on_error,
      on_completed
    });
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
