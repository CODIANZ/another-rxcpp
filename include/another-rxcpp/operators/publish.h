#if !defined(__another_rxcpp_h_publish__)
#define __another_rxcpp_h_publish__

#include "../observables.h"

namespace another_rxcpp {
namespace operators {

inline auto publish()
{
  return [](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observables::connectable<>::create<OUT>(src);
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_publish__) */