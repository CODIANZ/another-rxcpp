#if !defined(__another_rxcpp_h_blocking_observable__)
#define __another_rxcpp_h_blocking_observable__

#include "../observable.h"
#include <algorithm>
#include <mutex>
#include <numeric>

namespace another_rxcpp {
namespace observables {

template <typename T = void> class blocking;
template <typename T> class calculable_blocking;

template <> class blocking<void> {
public:
  template <
    typename T,
    std::enable_if_t<!std::is_arithmetic<T>::value, bool> = true
  >
  static auto create(observable<T> src)
  {
    return blocking<T>(src);
  }

  template <
    typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, bool> = true
  >
  static auto create(observable<T> src)
  {
    return calculable_blocking<T>(src);
  }
};

class empty_error : public std::runtime_error {
public:
  empty_error(const std::string& msg) : runtime_error(msg) {}
};

template <typename T> class blocking : public observable<T> {
friend class blocking<>;
public:
  using value_type    = typename observable<T>::value_type;
  using source_type   = typename observable<T>::source_type;
  using source_sp     = typename observable<T>::source_sp;
  using observer_type = typename observable<T>::observer_type;

protected:
  observable<T> src_;

  auto subscribe_all() const {
    std::vector<value_type> values;
    std::exception_ptr err = nullptr;
    auto sbsc = src_.subscribe({
      .on_next = [&](value_type x){
        values.push_back(std::move(x));
      },
      .on_error = [&](std::exception_ptr e){
        err = e;
      }
    });
    while(sbsc.is_subscribed()) {}
    if(err) std::rethrow_exception(err);
    return values;
  }

  blocking(observable<T> src) : src_(src) {}

public:

  virtual subscription subscribe(observer_type ob) const override {
    try{
      auto values = subscribe_all();
      std::for_each(
        std::cbegin(values),
        std::cend(values),
        [ob](auto it){
          ob.on_next(std::move(it));
        });
      ob.on_completed();
    }
    catch(...){
      ob.on_error(std::current_exception());
    }
    return subscription();
  }

  value_type first() const {
    auto  values = subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return values.front();
  }

  value_type last() const {
    auto  values = subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return values.back();
  }

  std::size_t count() const {
    auto  values = subscribe_all();
    return values.size();
  }
};

template <typename T> class calculable_blocking : public blocking<T>
{
friend class blocking<>;

public:
  using value_type  = typename blocking<T>::value_type;

private:
  using base   = blocking<T>;

  calculable_blocking(observable<T> src) : blocking<T>(src) {}

public:
  value_type sum() const {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return std::accumulate(std::cbegin(values), std::cend(values), 0);
  }

  double average() const {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return static_cast<double>(sum()) / static_cast<double>(values.size());
  }

  value_type max() const {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return *std::max_element(std::cbegin(values), std::cend(values));
  }

  value_type min() const {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return *std::min_element(std::cbegin(values), std::cend(values));
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_blocking_observable__) */
