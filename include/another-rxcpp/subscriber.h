#if !defined(__another_rxcpp_h_subscriber__)
#define __another_rxcpp_h_subscriber__

#include "observer.h"
#include "internal/source/source_base.h"
#include "internal/tools/private_access.h"
#include <cassert>

namespace another_rxcpp {

template <typename T> class subscriber {
friend class internal::private_access::subscriber;

public:
  using value_type    = T;
  using observer_type = observer<value_type>;

private:
  using source_base = internal::source_base;
  using source_sp   = typename source_base::sp;
  using observer_sp = typename observer_type::sp;
  using source_wp   = std::weak_ptr<source_base>;
  using observer_wp = std::weak_ptr<observer_type>;

  struct member {
    source_sp   source_;
    observer_wp observer_;
    std::vector<source_sp>  upstreams_;
    std::recursive_mutex  mtx_;
  };
  std::shared_ptr<member> m_;

  template <typename X> void add_upstream(X upstream) const noexcept {
    std::lock_guard<std::recursive_mutex> lock(m_->mtx_);
    m_->upstreams_.push_back(std::dynamic_pointer_cast<source_base>(upstream));
  }

  void unsubscribe_upstreams() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(m_->mtx_);
    for(auto it : m_->upstreams_){
      auto u = it;
      if(u){
        u->unsubscribe();
      }
    }
    m_->upstreams_.clear();
  }

public:
  subscriber() noexcept :
    m_(std::make_shared<member>()) {}

  template <typename SOURCE_SP>
    subscriber(SOURCE_SP source, observer_sp observer) noexcept :
      m_(std::make_shared<member>())
  {
    m_->source_ = std::dynamic_pointer_cast<source_base>(source);
    m_->observer_ = observer;
  }

  void on_next(const value_type& value) const noexcept {
    auto s = m_->source_;
    auto o = m_->observer_.lock();
    if(!is_subscribed()){
      unsubscribe();
      return;
    }
    if(s){
      s->on_next_function([&]() {
        if(o && o->on_next){
          o->on_next(value);
        }
      });
    }
  }


  void on_error(std::exception_ptr err) const noexcept {
    unsubscribe_upstreams();
    auto s = m_->source_;
    auto o = m_->observer_.lock();
    if(s){
      s->on_error_function([o, err](){
        if(o){
          assert(o->on_error);
          if(o->on_error){
            o->on_error(err);
          }
        }
      });
    }
    unsubscribe();
  }

  void on_completed() const noexcept {
    unsubscribe_upstreams();
    auto s = m_->source_;
    auto o = m_->observer_.lock();
    if(s){
      s->on_completed_function([o](){
        if(o && o->on_completed){
          o->on_completed();
        }
      });
    }
    unsubscribe();
  }

  bool is_subscribed() const noexcept {
    std::lock_guard<std::recursive_mutex> lock(m_->mtx_);
    auto s = m_->source_;
    return s ? s->is_subscribed() : false;
  }

  void unsubscribe() const noexcept {
    unsubscribe_upstreams();
    std::lock_guard<std::recursive_mutex> lock(m_->mtx_);
    auto s = m_->source_;
    if(s){
      s->unsubscribe();
    }
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscriber__) */
