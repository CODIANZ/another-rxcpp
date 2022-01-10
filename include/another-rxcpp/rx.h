#if !defined(__h_another_rxcpp__)
#define __h_another_rxcpp__

#include "observable.h"
#include "observables.h"
#include "subjects.h"
#include "operators.h"
#include "schedulers.h"

#if defined(SUPPORTS_RXCPP_COMPATIBLE)
namespace rxcpp {
  using namespace another_rxcpp;
  using namespace another_rxcpp::observables;
  using namespace another_rxcpp::schedulers;
  namespace schedulers  = another_rxcpp::schedulers;
  namespace subjects    = another_rxcpp::subjects;
}
#endif

#endif /* !defined(__h_another_rxcpp__) */