#if !defined(__another_rxcpp_h_subscriber__)
#define __another_rxcpp_h_subscriber__

#include "observer.h"

namespace another_rxcpp {

template <typename T> using subscriber = observer<T>;

} /* namespace another_rxcpp */
#endif /* !defined(__another_rxcpp_h_subscriber__) */
