#if !defined(__h_last__)
#define __h_last__

#include "../observable.h"
#include "take_last.h"

namespace another_rxcpp {
namespace operators {

inline auto last()
{
  return take_last(1);
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__h_last__) */
