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
  struct member {
    std::mutex              mtx_;
    source_sp               upstream_;
    std::vector<value_type> values_;
    std::exception_ptr      error_;
  };
  std::shared_ptr<member> m_;

  void subscribe_all() const {
    auto m = m_;
    std::lock_guard<std::mutex> lock(m->mtx_);
    if(!m->upstream_) return;
    auto sbsc = m->upstream_->subscribe({
      .on_next = [m](value_type x){
        m->values_.push_back(x);
      },
      .on_error = [&](std::exception_ptr e){
        m->error_ = e;
      }
    });
    while(sbsc.is_subscribed()) {}
    m->upstream_.reset();
  }

  blocking(observable<T> src) :
    m_(std::make_shared<member>())
  {
    m_->upstream_ = src.create_source();
    m_->error_ = nullptr;
  }

public:

  virtual subscription subscribe(observer_type ob) const override {
    subscribe_all();
    if(m_->error_){
      ob.on_error(m_->error_);
    }
    else{
      std::for_each(
        std::cbegin(m_->values_),
        std::cend(m_->values_),
        [ob](auto it){
          ob.on_next(std::move(it));
        });
      ob.on_completed();
    }
    return subscription();
  }

  value_type first() const {
    subscribe_all();
    if(m_->error_) std::rethrow_exception(m_->error_);
    if(m_->values_.empty()) throw empty_error("empty");
    return m_->values_.front();
  }

  value_type last() const {
    subscribe_all();
    if(m_->error_) std::rethrow_exception(m_->error_);
    if(m_->values_.empty()) throw empty_error("empty");
    return m_->values_.back();
  }

  std::size_t count() const {
    subscribe_all();
    if(m_->error_) std::rethrow_exception(m_->error_);
    return m_->values_.size();
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
    base::subscribe_all();
    auto m = base::m_;
    if(m->error_) std::rethrow_exception(m->error_);
    if(m->values_.empty()) throw empty_error("empty");
    return std::accumulate(std::cbegin(m->values_), std::cend(m->values_), 0);
  }

  double average() const {
    base::subscribe_all();
    auto m = base::m_;
    if(m->error_) std::rethrow_exception(m->error_);
    if(m->values_.empty()) throw empty_error("empty");
    return static_cast<double>(sum()) / static_cast<double>(m->values_.size());
  }

  value_type max() const {
    base::subscribe_all();
    auto m = base::m_;
    if(m->error_) std::rethrow_exception(m->error_);
    if(m->values_.empty()) throw empty_error("empty");
    return *std::max_element(std::cbegin(m->values_), std::cend(m->values_));
  }

  value_type min() const {
    base::subscribe_all();
    auto m = base::m_;
    if(m->error_) std::rethrow_exception(m->error_);
    if(m->values_.empty()) throw empty_error("empty");
    return *std::min_element(std::cbegin(m->values_), std::cend(m->values_));
  }
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_blocking_observable__) */
