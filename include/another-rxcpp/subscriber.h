#if !defined(__h_subscriber__)
#define __h_subscriber__

#include "observer.h"
#include "internal/source/source_base.h"
#include <cassert>

namespace another_rxcpp {

template <typename T> class subscriber {
public:
  using value_type    = T;
  using observer_type = observer<value_type>;

private:
  std::weak_ptr<source_base>    source_;
  std::weak_ptr<observer_type>  observer_;

public:
  subscriber() = default;
  template <typename SOURCE_SP>
    subscriber(SOURCE_SP source, typename observer_type::sp observer) :
      source_(source->shared_from_this()), observer_(observer) {}

  template <typename U> void on_next(U&& value) const {
    auto s = source_.lock();
    auto o = observer_.lock();
    auto v = std::forward<U>(value);
    if(s){
      s->on_next_function([o, &v](){
        if(o && o->on_next){
          o->on_next(std::move(v));
        }
      });
    }
  }

  void on_error(std::exception_ptr err) const {
    auto s = source_.lock();
    auto o = observer_.lock();
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
  }

  void on_completed() const {
    auto s = source_.lock();
    auto o = observer_.lock();
    if(s){
      s->on_completed_function([o](){
        if(o && o->on_completed){
          o->on_completed();
        }
      });
    }
  }

  bool is_subscribed() const {
    auto s = source_.lock();
    return s ? s->is_subscribed() : false;
  }

  void unsubscribe() const {
    auto s = source_.lock();
    if(s){
      s->unsubscribe();
    }
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_subscriber__) */
