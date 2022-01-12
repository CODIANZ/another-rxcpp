#if !defined(__another_rxcpp_h_blocking_observable__)
#define __another_rxcpp_h_blocking_observable__

#include "../observable.h"
#include "../subjects/subject.h"
#include "../operators/take.h"
#include "../internal/tools/shared_with_will.h"
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
  struct member {
    std::mutex              mtx_;
    std::size_t             read_;
    std::condition_variable cond_;
    subjects::subject<value_type> sbj_;
    subscription            subscription_;
    bool                    done_;
  };
  shared_with_will<member> m_;

  blocking(observable<T> src) :
    m_(std::make_shared<member>(), [](auto x){
      x->subscription_.unsubscribe();
      x->done_ = true;
      x->cond_.notify_one();
    })
  {
    auto m = m_.capture_element();
    m->done_ = false;
    std::thread([m, src](){
      m->subscription_ = src.subscribe({
        .on_next = [m](value_type x){
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0 || m->done_; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.get_subscriber().on_next(std::move(x));
        },
        .on_error = [m](std::exception_ptr err){
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.get_subscriber().on_error(err);
        },
        .on_completed = [m]() {
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.get_subscriber().on_completed();
        }
      });
    }).detach();
  }

  bool subscribe_one(value_type& value) const {
    struct _result {
      std::exception_ptr  err = nullptr;
      bool        gotValue = false;
      value_type  value;
      bool        bDone = false;
      std::mutex  mtx;
      std::condition_variable cond;
    };
    auto result = std::make_shared<_result>();
    std::exception_ptr  err = nullptr;
    bool gotValue = false;
    auto o = m_->sbj_.get_observable() | operators::take(1);
    auto sbsc = o.subscribe({
      .on_next = [result](value_type x){
        result->value = std::move(x);
        result->gotValue = true;
        std::lock_guard<std::mutex> lock(result->mtx);
        result->bDone = true;
        result->cond.notify_one();
      },
      .on_error = [result](std::exception_ptr e){
        result->err = e;
        std::lock_guard<std::mutex> lock(result->mtx);
        result->bDone = true;
        result->cond.notify_one();
      },
      .on_completed = [result](){
        std::lock_guard<std::mutex> lock(result->mtx);
        result->bDone = true;
        result->cond.notify_one();
      }
    });
    {
      std::lock_guard<std::mutex> lock(m_->mtx_);
      m_->read_++;
      m_->cond_.notify_one();
    }
    {
      std::unique_lock<std::mutex> lock(result->mtx);
      result->cond.wait(lock, [result]{ return result->bDone; });
    }
    if(result->err) std::rethrow_exception(result->err);
    if(!result->gotValue) return false;
    value = std::move(result->value);
    return true;
  }

  auto subscribe_all() const {
    std::vector<value_type> values;
    value_type value;
    while(subscribe_one(value)){
      values.push_back(std::move(value));
    }
    return values;
  }

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
    value_type value;
    if(!subscribe_one(value)) throw empty_error("empty");
    return value;
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
    auto sum = std::accumulate(std::cbegin(values), std::cend(values), 0);
    return static_cast<double>(sum) / static_cast<double>(values.size());
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
