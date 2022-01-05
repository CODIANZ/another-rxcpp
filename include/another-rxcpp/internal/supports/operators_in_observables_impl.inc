#include "../../operators.h"
#include "../../observables/connectable.h"

namespace another_rxcpp {
  template <typename T>
    template<typename F>
      auto observable<T>::flat_map(F f) const
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::flat_map<std::function<OUT(value_type)>>(f);
  }

  template <typename T>
    template <typename F>
      auto observable<T>::map(F f) const
  {
    using OUT = decltype(f(std::declval<value_type>()));
    return *this | operators::map<std::function<OUT(value_type)>>(f);
  }

  template <typename T>
    auto observable<T>::take(std::size_t n) const
  {
    return *this | operators::take(n);
  }

  template <typename T>
    auto observable<T>::publish() const
  {
    return *this | operators::publish();
  }

  template <typename T>
    template <typename F>
      auto observable<T>::on_error_resume_next(F f) const
  {
    using OUT = observable<value_type>;
    return *this | operators::on_error_resume_next<std::function<OUT(std::exception_ptr)>>(f);
  }

  template <typename T>
    auto observable<T>::retry(std::size_t n) const
  {
    return *this | operators::retry();
  }

  template <typename T>
    auto observable<T>::observe_on(scheduler::creator_fn sccr) const
  {
    return *this | operators::observe_on(sccr);
  }

  template <typename T>
    auto observable<T>::subscribe_on(scheduler::creator_fn sccr) const
  {
    return *this | operators::subscribe_on(sccr);
  }

  template <typename T>
    template <typename OB>
      auto observable<T>::take_until(OB ob) const
  {
    return *this | operators::take_until(ob);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::merge(scheduler::creator_fn sccr, ARGS...args) const
  {
    return *this | operators::merge(sccr, args...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::merge(ARGS...args) const
  {
    return *this | operators::merge(args...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::amb(scheduler::creator_fn sccr, ARGS...args) const
  {
    return *this | operators::amb(sccr, args...);
  }

  template <typename T>
    template <typename...ARGS>
      auto observable<T>::amb(ARGS...args) const
  {
    return *this | operators::amb(args...);
  }

  template <typename T>
    auto observable<T>::distinct_until_changed() const
  {
    return *this | operators::distinct_until_changed();
  }

  template <typename T>
    auto observable<T>::tap(
      std::function<void(value_type)>         on_next,
      std::function<void(std::exception_ptr)> on_error,
      std::function<void()>                   on_completed
    ) const
  {
    return *this | operators::tap({
      .on_next      = on_next,
      .on_error     = on_error,
      .on_completed = on_completed
    });
  }

  template <typename T>
    auto observable<T>::timeout(std::chrono::milliseconds d) const
  {
    return *this | operators::timeout(d);
  }

  template <typename T>
    auto observable<T>::delay(std::chrono::milliseconds d) const
  {
    return *this | operators::delay(d);
  }
  
  template <typename T>
    auto observable<T>::last() const
  {
    return *this | operators::last();
  }
  
  template <typename T>
    auto observable<T>::finally(std::function<void()> on_finally) const
  {
    return *this | operators::finally(on_finally);
  }
  
  template <typename T>
    auto observable<T>::take_last(std::size_t n) const
  {
    return *this | operators::take_last(n);
  }

} /* namespace another_rxcpp */
