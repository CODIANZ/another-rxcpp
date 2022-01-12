#if !defined(__another_rxcpp_h_another_rxcpp__)
#define __another_rxcpp_h_another_rxcpp__

#include "observable.h"
#include "observables.h"
#include "subjects.h"
#include "operators.h"
#include "schedulers.h"
#include "utils.h"

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
namespace rxcpp {
  using namespace another_rxcpp;
  using namespace another_rxcpp::observables;
  using namespace another_rxcpp::schedulers;
  namespace schedulers  = another_rxcpp::schedulers;
  namespace subjects    = another_rxcpp::subjects;
  namespace sources     = another_rxcpp::observables;
  namespace util {}
}
#endif

#endif /* !defined(__another_rxcpp_h_another_rxcpp__) */