#if defined(SUPPORTS_RXCPP_COMPATIBLE)

#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_rxcpp_compatible() {
  log() << "test_rxcpp_compatible -- begin" << std::endl;

  auto o = ovalue(1)
  .as_dynamic()
  .subscribe([](auto x){
    std::cout << x << std::endl;
  }, [](auto){
    std::cout << "error" << std::endl;
  }, [](){
    std::cout << "completed" << std::endl;
  });

  log() << "test_rxcpp_compatible -- end" << std::endl << std::endl;
}

#endif /* defined(SUPPORTS_RXCPP_COMPATIBLE) */