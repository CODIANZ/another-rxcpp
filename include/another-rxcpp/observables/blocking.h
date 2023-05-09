#if !defined(__another_rxcpp_h_blocking_observable__)
#define __another_rxcpp_h_blocking_observable__

#include "../observable.h"
#include "../subjects/subject.h"
#include "../operators/take.h"
#include <algorithm>
#include <mutex>
#include <numeric>
#include <thread>
#include <atomic>

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
  static auto create(observable<T> src) noexcept
  {
    return blocking<T>(src);
  }

  template <
    typename T,
    std::enable_if_t<std::is_arithmetic<T>::value, bool> = true
  >
  static auto create(observable<T> src) noexcept
  {
    return calculable_blocking<T>(src);
  }
};

class empty_error : public std::runtime_error {
public:
  empty_error(const std::string& msg) noexcept : runtime_error(msg) {}
};

template <typename T> class blocking : public observable<T> {
friend class blocking<>;
public:
  using value_type    = typename observable<T>::value_type;
  using observer_type = typename observable<T>::observer_type;
  using this_type     = blocking<value_type>;

protected:
  mutable observable<value_type>  src_;
  mutable std::deque<value_type>  queue_;
  mutable std::exception_ptr      error_;
  mutable bool                    subscribed_;

  blocking(observable<value_type> src) noexcept :
    src_(src), error_(nullptr), subscribed_(false) {}

  void  subscribe_all() const {
    if(subscribed_) return;
    subscribed_ = true;
    src_.subscribe(
      [&](const value_type& x){
        queue_.push_back(x);
      },
      [&](std::exception_ptr err){
        error_ = err;
      },
      []{}
    );
  }

public:

  virtual subscription subscribe(observer_type ob) const noexcept override {
    subscribe_all();
    std::for_each(std::begin(queue_), std::end(queue_), [ob](auto&& x){
      ob.on_next(x);
    });
    if(error_){
      ob.on_error(error_);
    }
    else{
      ob.on_completed();
    }
    return subscription(
      []{},
      []{
        return false;
      }
    );
  }

  value_type first() const noexcept(false) {
    subscribe_all();
    if(queue_.size() == 0){
      if(error_){
        std::rethrow_exception(error_);
      }
      else{
        throw empty_error("empty");
      }
    }
    else{
      auto x = queue_.front();
      queue_.pop_front();
      return x;
    }
  }

  value_type last() const noexcept(false) {
    subscribe_all();
    if(queue_.empty()){
      if(error_){
        std::rethrow_exception(error_);
      }
      else{
        throw empty_error("empty");
      }
    }
    else{
      auto x = queue_.back();
      queue_.pop_back();
      return x;
    }
  }

  std::size_t count() const noexcept(false) {
    subscribe_all();
    return queue_.size();
  }
};

template <typename T> class calculable_blocking : public blocking<T>
{
friend class blocking<>;

public:
  using value_type  = typename blocking<T>::value_type;

private:
  using base   = blocking<T>;

  calculable_blocking(observable<T> src) noexcept : blocking<T>(src) {}

public:
  value_type sum() const noexcept(false) {
    base::subscribe_all();
    if(base::error_) std::rethrow_exception(base::error_);
    else if(base::queue_.empty()) throw empty_error("empty");
    return std::accumulate(std::cbegin(base::queue_), std::cend(base::queue_), 0);
  }

  double average() const noexcept(false) {
    base::subscribe_all();
    if(base::error_) std::rethrow_exception(base::error_);
    else if(base::queue_.empty()) throw empty_error("empty");
    auto sum = std::accumulate(std::cbegin(base::queue_), std::cend(base::queue_), 0);
    return static_cast<double>(sum) / static_cast<double>(base::queue_.size());
  }

  value_type max() const noexcept(false) {
    base::subscribe_all();
    if(base::error_) std::rethrow_exception(base::error_);
    else if(base::queue_.empty()) throw empty_error("empty");
    return *std::max_element(std::cbegin(base::queue_), std::cend(base::queue_));
  }

  value_type min() const noexcept(false) {
    base::subscribe_all();
    if(base::error_) std::rethrow_exception(base::error_);
    else if(base::queue_.empty()) throw empty_error("empty");
    return *std::min_element(std::cbegin(base::queue_), std::cend(base::queue_));
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_blocking_observable__) */
