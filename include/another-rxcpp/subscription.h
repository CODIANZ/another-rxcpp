#if !defined(__another_rxcpp_h_subscription__)
#define __another_rxcpp_h_subscription__

#include "internal/tools/fn.h"

namespace another_rxcpp {

class subscription {
private:
  struct member {
    const internal::fn<void()> unsubscribe_;
    const internal::fn<bool()> is_subscribed_;
    template <typename Unsb, typename Issb>
      member(Unsb&& unsb, Issb&& issb) noexcept :
        unsubscribe_(std::forward<Unsb>(unsb)),
        is_subscribed_(std::forward<Issb>(issb)) {}
  };

  std::shared_ptr<member> m_;

public:
  subscription() noexcept = default;
  
  template <typename Unsb, typename Issb>
    subscription(Unsb&& unsb, Issb&& issb) noexcept :
      m_(std::make_shared<member>(
        std::forward<Unsb>(unsb),
        std::forward<Issb>(issb)
      )){}

  subscription(const subscription& src) noexcept : m_(src.m_){}

  const subscription& operator = (const subscription& src) {
    m_ = src.m_;
    return *this;
  }

  void unsubscribe() const noexcept {
    auto m = m_;
    if(m && m->unsubscribe_) m->unsubscribe_();
  }

  bool is_subscribed() const noexcept {
    auto m = m_;
    return m && m->is_subscribed_ ? m->is_subscribed_() : false;
  }
};

} /* namespace another_rxcpp */
#endif /* !defined(__another_rxcpp_h_subscription__) */
