#if !defined(__another_rxcpp_h_util__)
#define __another_rxcpp_h_util__

#include <tuple>
#include <type_traits>

namespace another_rxcpp { namespace internal {

template <typename T> struct strip_const_reference {
  using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
};

template <typename RET, typename...ARGS>
struct lambda_traits_impl {
  using return_type  = RET;
  using args         = std::tuple<ARGS...>;
};

template <typename T>
  struct lambda_traits :
    lambda_traits<decltype(&T::operator())> {};

template <typename FUNC, typename RET, typename...ARGS>
  struct lambda_traits<RET(FUNC::*)(ARGS...) const> :
    lambda_traits_impl<RET, ARGS...> {};

template <typename FUNC, typename RET, typename...ARGS>
  struct lambda_traits<RET(FUNC::*)(ARGS...)> :
    lambda_traits_impl<RET, ARGS...> {};

template <typename FUNC>
  using lambda_invoke_result_t = typename lambda_traits<FUNC>::return_type;

template <typename FUNC, std::size_t N>
  using lambda_arg_t = typename std::tuple_element<N, typename lambda_traits<FUNC>::args>::type;

template <typename SP>
  inline auto to_weak(SP sp) noexcept {
    return std::weak_ptr<typename SP::element_type>(sp);
  }

template <typename T>
  auto to_shared(T&& value) noexcept {
    return std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(value));
  }

}} /* namespace another_rxcpp::internal */

#endif /* !defined(__another_rxcpp_h_util__) */
