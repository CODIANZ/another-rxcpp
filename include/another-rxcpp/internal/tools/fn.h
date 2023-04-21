#if !defined(__another_rxcpp_h_function__)
#define __another_rxcpp_h_function__

#include <functional>

namespace another_rxcpp { namespace internal {

template <typename F> using fn    = std::function<F>;


}} /* namespace another_rxcpp::internal */
#endif /* !defined(__another_rxcpp_h_function__) */
