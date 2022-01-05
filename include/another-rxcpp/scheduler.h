#if !defined(__h_scheduler__)
#define __h_scheduler__

#include "internal/tools/any_sp_keeper.h"
#include <memory>

namespace another_rxcpp {

class scheduler_interface {
public:
  using function_type = std::function<void()>;
  scheduler_interface() = default;
  virtual ~scheduler_interface() = default;
  virtual void run(function_type, any_sp_keeper) = 0;
};

class scheduler{
public:
  using function_type = typename scheduler_interface::function_type;
  using creator_fn    = std::function<scheduler()>;

private:
  using isp = std::shared_ptr<scheduler_interface>;
  isp interface_;

protected:

public:
  template <typename ISP> scheduler(ISP isp) :
    interface_(std::dynamic_pointer_cast<scheduler_interface>(isp)) {}

  ~scheduler() {
  }

  void run(function_type f) const {
    interface_->run(f, any_sp_keeper::create(interface_));
  }
};

} /* namespace another_rxcpp */

#endif /* !defined(__h_scheduler__) */