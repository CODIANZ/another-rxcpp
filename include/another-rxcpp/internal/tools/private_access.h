#if !defined(__another_rxcpp_h_private_access__)
#define __another_rxcpp_h_private_access__

namespace another_rxcpp { namespace internal { namespace private_access {

class observable {
public:
  template <typename OBSERVABLE> static auto create_source(const OBSERVABLE& o) noexcept {
    return o.create_source();
  }
};

class subscriber {
public:
  template <typename SUBSCRIBER, typename UPSTREAM> static void add_upstream(const SUBSCRIBER& s, UPSTREAM&& upstream) noexcept {
    s.add_upstream(std::forward<UPSTREAM>(upstream));
  }
  template <typename SUBSCRIBER> static void unsubscribe_upstreams(const SUBSCRIBER& s) noexcept {
    s.unsubscribe_upstreams();
  }
};

}}} /* namespace another_rxcpp::internal::private_access */

#endif /* !defined(__another_rxcpp_h_private_access__) */
