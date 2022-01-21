#include "../../operators.h"
#include "../../observables/connectable.h"

namespace another_rxcpp {
  template <typename T>
    template<typename F, typename...ARGS>
      auto observable<T>::flat_map(F f, ARGS...args) const
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::flat_map<std::function<OUT(value_type)>>(f, std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename F, typename...ARGS>
      auto observable<T>::map(F f, ARGS...args) const
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::map<std::function<OUT(value_type)>>(f, std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename F, typename...ARGS>
      auto observable<T>::tap(F f, ARGS...args) const
  {
    return *this | operators::tap<std::function<void(value_type)>>(f, std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool>>
      auto observable<T>::zip(OB ob, ARGS...args) const
  {
    return *this | operators::zip(std::forward<OB>(ob), std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename F, typename...ARGS, std::enable_if_t<!is_observable<F>::value, bool>>
      auto observable<T>::zip(F f, ARGS...args) const
  {
    using fargs = operators::zip_internal::function_parameter<observable<value_type>, ARGS...>;
    using fret = decltype(fargs::feval(f));
    using FTYPE = typename fargs::template ftype<fret>;
    return *this | operators::zip<FTYPE, ARGS...>(std::forward<F>(f), std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::take(ARGS...args) const
  {
    return *this | operators::take(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::on_error_resume_next(ARGS...args) const
  {
    return *this | operators::on_error_resume_next(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::observe_on(ARGS...args) const
  {
    return *this | operators::observe_on(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::retry(ARGS...args) const
  {
    return *this | operators::retry(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::subscribe_on(ARGS...args) const
  {
    return *this | operators::subscribe_on(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::skip_until(ARGS...args) const
  {
    return *this | operators::skip_until(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::skip_while(ARGS...args) const
  {
    return *this | operators::skip_while(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::take_last(ARGS...args) const
  {
    return *this | operators::take_last(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::take_until(ARGS...args) const
  {
    return *this | operators::take_until(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::take_while(ARGS...args) const
  {
    return *this | operators::take_while(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::merge(ARGS...args) const
  {
    return *this | operators::merge(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::amb(ARGS...args) const
  {
    return *this | operators::amb(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::timeout(ARGS...args) const
  {
    return *this | operators::timeout(std::forward<ARGS>(args)...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::delay(ARGS...args) const
  {
    return *this | operators::delay(std::forward<ARGS>(args)...);
  }
    
  template <typename T>
    template <typename...ARGS>
      auto observable<T>::finally(ARGS...args) const
  {
    return *this | operators::finally(std::forward<ARGS>(args)...);
  }
  
  template <typename T>
    auto observable<T>::publish() const
  {
    return *this | operators::publish();
  }

  template <typename T>
    auto observable<T>::as_blocking() const
  {
    return *this | operators::blocking();
  }

  template <typename T>
    auto observable<T>::distinct_until_changed() const
  {
    return *this | operators::distinct_until_changed();
  }

  template <typename T>
    auto observable<T>::last() const
  {
    return *this | operators::last();
  }

} /* namespace another_rxcpp */
