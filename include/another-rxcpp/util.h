#if !defined(__h_util__)
#define __h_util__

#include <functional>

namespace another_rxcpp {

template <typename T> struct strip_const_referece {
  using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_util__) */
