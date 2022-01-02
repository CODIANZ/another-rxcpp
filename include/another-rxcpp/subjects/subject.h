#if !defined(__h_subject__)
#define __h_subject__

#include "../internal/source/source.h"
#include "../observables.h"
#include "../observable.h"

namespace another_rxcpp {
namespace subjects {

template <typename T> class subject {
public:
  using value_type = T;
  using source_type       = observables::connectable<value_type>;
  using observer_type     = observer<value_type>;
  using subscriber_type   = subscriber<value_type>;

private:
  source_type     source_;
  subscriber_type subscriber_;
  subscription    subscription_;

protected:

public:
  subject()
  {
    source_ = observable<>::create<value_type>([&](subscriber_type s){
      subscriber_ = s;
    }) | operators::publish();
    subscription_ = source_.subscribe({
      .on_next = [](value_type&&) {},
      .on_error = [](std::exception_ptr) {},
      .on_completed = []() {},
    });
  }

  virtual ~subject() {
    subscription_.unsubscribe();
  }

  auto as_subscriber() {
    return subscriber_;
  }

  auto as_observable() {
    return source_;
  }
};

} /* namespace subjects */
} /* namespace another_rxcpp */

#endif /* !defined(__h_subject__) */
