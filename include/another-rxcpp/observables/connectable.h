#if !defined(__h_connectable_observable__)
#define __h_connectable_observable__

#include "../observable.h"
#include "../internal/source/connectable_source.h"

namespace another_rxcpp {
namespace observables {

template <typename T = void> class connectable;

template <> class connectable<void> {
public:
  template <typename T> static auto create(observable<T> src) {
    return connectable<T>(src);
  }
};

template <typename T> class connectable : public observable<T> {
friend class connectable<>;

public:
  connectable() = default; /* do not use this normally! */
  connectable(observable<T> src) : 
    observable<T>([src](){
      return std::dynamic_pointer_cast<
        typename observable<T>::source_type
      >(
        std::make_shared<connectable_source<T>>(
          src.create_source()
        )
      );
    })
  {}
};

} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__h_connectable_observable__) */
