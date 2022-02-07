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
  using source_type   = typename observable<T>::source_type;
  using source_sp     = typename observable<T>::source_sp;
  using observer_type = typename observable<T>::observer_type;

protected:
  struct member {
    std::mutex              mtx_;
    std::size_t             read_ = 0;
    std::condition_variable cond_;
    subjects::subject<value_type> sbj_;
    subscription            subscription_;
    source_sp               upstream_;
    bool                    done_ = false;
    std::atomic_bool        started_{false};
  };
  internal::shared_with_will<member>  m_;
  observable<value_type>    src_;

  blocking(observable<T> src) noexcept :
    m_(std::make_shared<member>(), [](auto x){
      if(x->started_.exchange(false)){
        {
          std::unique_lock<std::mutex> lock(x->mtx_);
          x->done_ = true;
          x->cond_.notify_one();
        }
        x->subscription_.unsubscribe();
        x->upstream_->unsubscribe();
      }
    }),
    src_(src)
  {}

  void start_subscribing_if_not() const noexcept {
    if(m_->started_.exchange(true)) return;
    m_->upstream_ = internal::private_access::observable::create_source(src_);
    auto m = m_.capture_element();
    std::thread([m](){
      m->subscription_ = m->upstream_->subscribe({
        [m](const value_type& x){
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0 || m->done_; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.as_subscriber().on_next(x);
        },
        [m](std::exception_ptr err){
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.as_subscriber().on_error(err);
        },
        [m]() {
          std::unique_lock<std::mutex> lock(m->mtx_);
          m->cond_.wait(lock, [m]{ return m->read_ > 0; });
          if(m->done_) return;
          m->read_--;
          m->sbj_.as_subscriber().on_completed();
        }
      });
    }).detach();
  }

  bool subscribe_one(value_type& value) const noexcept(false) {
    struct _result {
      std::exception_ptr          err = nullptr;
      std::shared_ptr<value_type> pvalue;
      bool                        bDone = false;
      std::mutex                  mtx;
      std::condition_variable     cond;
    };
    auto result = std::make_shared<_result>();
    auto o = m_->sbj_.as_observable() | operators::take(1);
    auto sbsc = o.subscribe({
      [result](const value_type& x){
        std::lock_guard<std::mutex> lock(result->mtx);
        result->pvalue = std::make_shared<value_type>(x);
        result->bDone = true;
        result->cond.notify_one();
      },
      [result](std::exception_ptr e){
        result->err = e;
        std::lock_guard<std::mutex> lock(result->mtx);
        result->bDone = true;
        result->cond.notify_one();
      },
      [result](){
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
    start_subscribing_if_not();
    {
      std::unique_lock<std::mutex> lock(result->mtx);
      result->cond.wait(lock, [result]{ return result->bDone; });
    }
    if(result->err) std::rethrow_exception(result->err);
    if(!result->pvalue) return false;
    value = std::move(*result->pvalue);
    return true;
  }

  auto subscribe_all() const noexcept(false) {
    std::vector<value_type> values;
    value_type value;
    while(subscribe_one(value)){
      values.push_back(std::move(value));
    }
    return values;
  }

public:

  virtual subscription subscribe(observer_type&& ob) const noexcept override {
    try{
      auto values = subscribe_all();
      std::for_each(
        std::cbegin(values),
        std::cend(values),
        [&ob](const auto& it){
          ob.on_next(it);
        });
      ob.on_completed();
    }
    catch(...){
      ob.on_error(std::current_exception());
    }
    return subscription();
  }

  value_type first() const noexcept(false) {
    value_type value;
    if(!subscribe_one(value)) throw empty_error("empty");
    return value;
  }

  value_type last() const noexcept(false) {
    auto  values = subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return values.back();
  }

  std::size_t count() const noexcept(false) {
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

  calculable_blocking(observable<T> src) noexcept : blocking<T>(src) {}

public:
  value_type sum() const noexcept(false) {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return std::accumulate(std::cbegin(values), std::cend(values), 0);
  }

  double average() const noexcept(false) {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    auto sum = std::accumulate(std::cbegin(values), std::cend(values), 0);
    return static_cast<double>(sum) / static_cast<double>(values.size());
  }

  value_type max() const noexcept(false) {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return *std::max_element(std::cbegin(values), std::cend(values));
  }

  value_type min() const noexcept(false) {
    auto  values = base::subscribe_all();
    if(values.empty()) throw empty_error("empty");
    return *std::min_element(std::cbegin(values), std::cend(values));
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_blocking_observable__) */
