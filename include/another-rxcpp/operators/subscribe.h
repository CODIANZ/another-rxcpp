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

template <typename ON_NEXT>
  auto subscribe(
    ON_NEXT                 next,
    observer<>::error_t     error,
    observer<>::completed_t completed) noexcept
{
  return [next, error, completed](auto src){
    using observer_type = typename decltype(src)::observer_type;
    return src.subscribe({next, error, completed});
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscribe__) */