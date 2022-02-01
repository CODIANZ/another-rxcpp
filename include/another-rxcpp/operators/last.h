#if !defined(__another_rxcpp_h_last__)
#define __another_rxcpp_h_last__

#include "../observable.h"
#include "take_last.h"

namespace another_rxcpp {
namespace operators {

inline auto last() noexcept
{
  return take_last(1);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_last__) */
