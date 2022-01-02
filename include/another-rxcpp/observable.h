#if !defined(__h_observable__)
#define __h_observable__

#include "internal/source/source.h"

namespace another_rxcpp {

template <typename T = void> class observable;

template <> class observable<void> {
public:
  template<typename T> static auto create(typename source<T>::creator_fn_t f){
    return observable<T>([f](){
      return source<>::create<T>(f);
    });
  }
};

class observable_base {
protected:
  template <typename T> static typename source<T>::sp execute_source_creator(observable<T>&& o){
    return o.source_creator_fn_();
  }
public:
  observable_base() = default;
  virtual ~observable_base() = default;
};

template <typename T> class observable : public observable_base {
friend observable_base;
public:
  using value_type    = T;
  using souce_type    = source<value_type>;
  using source_sp     = typename souce_type::sp;
  using observer_type = typename souce_type::observer_type;
  using source_creator_fn_t = std::function<source_sp()>;

private:
  mutable source_creator_fn_t source_creator_fn_;

protected:
  observable() = default;

public: 
  observable(source_creator_fn_t source_creator) :
    source_creator_fn_(source_creator) {}
  virtual ~observable() = default;

  virtual subscription subscribe(observer_type ob) {
    return source_creator_fn_()->subscribe(ob);
  }

  template <typename F> auto operator | (F f)
    -> decltype(f(std::declval<observable<value_type>>()))
  {
    /** F -> observable<OUT> f(observable<IN>) */
    return f(*this);
  }

  source_sp create_source() const { return source_creator_fn_(); }

  #if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
    #include "internal/supports/operators_in_observables_decl.inc"
  #endif /* defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) */

  #if defined(SUPPORTS_RXCPP_COMPATIBLE)

    auto as_dynamic() -> observable<value_type>
    {
      return *this;
    }
  
    subscription subscribe(
      std::function<void(value_type)>         on_next = {},
      std::function<void(std::exception_ptr)> on_error = {},
      std::function<void()>                   on_completed = {}
    ) {
      return source_creator_fn_()->subscribe({
        .on_next      = on_next,
        .on_error     = on_error,
        .on_completed = on_completed
      });
    }
  
  #endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */
};

} /* namespace another_rxcpp */

#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE)
  #include "internal/supports/operators_in_observables_impl.inc"
#endif /* SUPPORTS_OPERATORS_IN_OBSERVABLE */

#endif /* !defined(__h_observable__) */
