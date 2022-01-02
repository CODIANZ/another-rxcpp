#include "../../operators.h"

namespace another_rxcpp {
  template <typename T>
    template<typename F>
      auto observable<T>::flat_map(F f)
        -> decltype(f(std::declval<value_type>()))
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::flat_map<std::function<OUT(value_type)>>(f);
  }

  template <typename T>
    template <typename F>
      auto observable<T>::map(F f)
        -> observable<decltype(f(std::declval<value_type>()))>
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::map<std::function<OUT(value_type)>>(f);
  }

  template <typename T>
    auto observable<T>::take(std::size_t n)
      -> observable<value_type>
  {
    return *this | operators::take(n);
  }

  template <typename T>
    auto observable<T>::publish()
      -> observable<value_type>
  {
    return *this | operators::publish();
  }

  template <typename T>
    template <typename F>
      auto observable<T>::on_error_resume_next(F f)
        -> observable<value_type>
  {
    using OUT = observable<value_type>;
    return *this | operators::on_error_resume_next<std::function<OUT(std::exception_ptr)>>(f);
  }

  template <typename T>
    auto observable<T>::retry()
      -> observable<value_type>
  {
    return *this | retry();
  }

  template <typename T>
    auto observable<T>::observe_on(scheduler scl)
      -> observable<value_type>
  {
    return *this | observe_on(scl);
  }

  template <typename T>
    auto observable<T>::subscribe_on(scheduler scl)
      -> observable<value_type>
  {
    return *this | subscribe_on(scl);
  }
} /* namespace another_rxcpp */
