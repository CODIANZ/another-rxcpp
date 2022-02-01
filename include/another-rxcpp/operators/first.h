#if !defined(__another_rxcpp_h_first__)
#define __another_rxcpp_h_first__

#include "../observable.h"
#include "take.h"

namespace another_rxcpp {
namespace operators {

inline auto first() noexcept
{
  return take(1);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_first__) */
