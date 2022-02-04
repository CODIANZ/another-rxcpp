#if !defined(__another_rxcpp_h_subscribe__)
#define __another_rxcpp_h_subscribe__

#include "../observer.h"

namespace another_rxcpp {
namespace operators {

template <typename T>
  auto subscribe(const observer<T>& ob)
{
  return [ob](auto&& src){
    return src.subscribe(std::move(ob));
  };
}

template <typename T>
  auto subscribe(observer<T>&& ob)
{
  return [ob = std::move(ob)](auto&& src){
    return src.subscribe(std::move(ob));
  };
}

template <typename ON_NEXT>
  auto subscribe(
    ON_NEXT&&                       next,
    const observer<>::error_t&      error,
    const observer<>::completed_t&  completed) noexcept
{
  return [next = std::forward<ON_NEXT>(next), error, completed](auto&& src){
    using src_type = typename internal::strip_const_referece<decltype(src)>::type;
    using observer_type = typename src_type::observer_type;
    return src.subscribe({
      std::move(next),
      std::move(error),
      std::move(completed)
    });
  };
}

} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_subscribe__) */