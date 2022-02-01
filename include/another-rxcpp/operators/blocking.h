#if !defined(__another_rxcpp_h_blocking__)
#define __another_rxcpp_h_blocking__

#include "../observables.h"

namespace another_rxcpp {
namespace operators {

inline auto blocking() noexcept
{
  return [](auto src){
    using OUT_OB = decltype(src);
    using OUT = typename OUT_OB::value_type;
    return observables::blocking<>::create<OUT>(src);
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_blocking__) */