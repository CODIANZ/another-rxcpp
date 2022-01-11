#if !defined(__another_rxcpp_h_subscriber__)
#define __another_rxcpp_h_subscriber__

#include "observer.h"
#include "internal/source/source_base.h"
#include <cassert>

namespace another_rxcpp {

template <typename T> class subscriber {
public:
  using value_type    = T;
  using observer_type = observer<value_type>;

private:
  using soruce_sp   = typename source_base::sp;
  using observer_sp = typename observer_type::sp;

  struct member {
    soruce_sp   source_;
    observer_sp observer_;
    std::vector<soruce_sp>  upstreams_;
    std::mutex  mtx_;
  };
  std::shared_ptr<member> m_;

public:
  subscriber() :
    m_(std::make_shared<member>()) {}

  template <typename SOURCE_SP>
    subscriber(SOURCE_SP source, typename observer_type::sp observer) :
      m_(std::make_shared<member>())
  {
    m_->source_ = source->shared_from_this();
    m_->observer_ = observer;
  }

  template <typename U> void on_next(U&& value) const {
    auto s = m_->source_;
    auto o = m_->observer_;
    auto v = std::forward<U>(value);
    if(!is_subscribed()){
      unsubscribe();
      return;
    }
    if(s){
      s->on_next_function([o, &v](){
        if(o && o->on_next){
          o->on_next(std::move(v));
        }
      });
    }
  }

  void on_error(std::exception_ptr err) const {
    auto s = m_->source_;
    auto o = m_->observer_;
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

  void on_completed() const {
    auto s = m_->source_;
    auto o = m_->observer_;
    if(s){
      s->on_completed_function([o](){
        if(o && o->on_completed){
          o->on_completed();
        }
      });
    }
    unsubscribe();
  }

  bool is_subscribed() const {
    std::lock_guard<std::mutex> lock(m_->mtx_);
    auto s = m_->source_;
    return s ? s->is_subscribed() : false;
  }

  void unsubscribe() const {
    std::lock_guard<std::mutex> lock(m_->mtx_);
    auto s = m_->source_;
    if(s){
      s->unsubscribe();
    }
    for(auto it : m_->upstreams_){
      auto u = it;
      if(u){
        u->unsubscribe();
      }
    }
    m_->upstreams_.clear();
    m_->observer_.reset();
    m_->source_.reset();
  }

  template <typename X> void add_upstream(X upstream) const {
    std::lock_guard<std::mutex> lock(m_->mtx_);
    m_->upstreams_.push_back(std::dynamic_pointer_cast<source_base>(upstream));
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscriber__) */
