#if !defined(__another_rxcpp_h_error__)
#define __another_rxcpp_h_error__

#include "../observable.h"

namespace another_rxcpp {
namespace observables {


template <typename T>
  inline auto error(std::exception_ptr err) noexcept
    -> observable<T>
{
  return observable<>::create<T>([err](subscriber<T> s){
    s.on_error(err);
  });
}

template <typename T, typename ERR>
  inline auto error(const ERR& err)
    -> observable<T>
{
  return error<T>(std::make_exception_ptr(err));
}


} /* namespace observables */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_error__) */