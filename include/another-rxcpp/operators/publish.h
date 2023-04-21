#if !defined(__another_rxcpp_h_publish__)
#define __another_rxcpp_h_publish__

#include "../observables/connectable.h"

namespace another_rxcpp {
namespace operators {

inline auto publish() noexcept
{
  return [](auto source){
    using Source = decltype(source);
    using Out = typename Source::value_type;
    return observables::connectable<>::create<Out>(source);
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_publish__) */