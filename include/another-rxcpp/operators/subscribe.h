#if !defined(__another_rxcpp_h_subscribe__)
#define __another_rxcpp_h_subscribe__

#include "../observer.h"

namespace another_rxcpp {
namespace operators {

template <typename T>
  auto subscribe(observer<T> ob)
{
  return [ob](auto src){
    return src.subscribe(ob);
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscribe__) */