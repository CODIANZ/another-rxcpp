#if !defined(__h_observable__)
#define __h_observable__

#include "source.h"

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
  source_creator_fn_t source_creator_fn_;

public: 
  observable(source_creator_fn_t source_creator) :
    source_creator_fn_(source_creator) {}
  virtual ~observable() = default;

  subscription subscribe(observer_type ob) {
    return source_creator_fn_()->subscribe(ob);
  }

  template <typename F> auto operator | (F f)
    -> decltype(f(std::declval<observable<value_type>>()))
  {
    /** F -> observable<OUT> f(observable<IN>) */
    return f(*this);
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_observable__) */
