#if !defined(__another_rxcpp_h_subscription__)
#define __another_rxcpp_h_subscription__

#include "internal/tools/fn.h"

namespace another_rxcpp {

class subscription {
private:
  struct member {
    const internal::fn<void()> unsubscribe_;
    const internal::fn<bool()> is_subscribed_;
  };

  std::shared_ptr<member> m_;

public:
  subscription() = default;
  subscription(
    const internal::fn<void()>& unsubscribe,
    const internal::fn<bool()>& is_subscribed
  ) noexcept : m_(std::make_shared<member>(member{
    .unsubscribe_   = unsubscribe,
    .is_subscribed_ = is_subscribed
  })) {}
  subscription(
    internal::fn<void()>&& unsubscribe,
    internal::fn<bool()>&& is_subscribed
  ) noexcept : m_(std::make_shared<member>(member{
    .unsubscribe_   = std::move(unsubscribe),
    .is_subscribed_ = std::move(is_subscribed)
  })) {}
  subscription(const subscription& src): m_(src.m_){}

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
