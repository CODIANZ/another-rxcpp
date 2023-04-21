#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/blocking.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observables/range.h>
#include <another-rxcpp/observables/iterate.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_blocking() {
  log() << "test_blocking -- begin" << std::endl;

  log() << "range 1 - 10" << std::endl;
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "  first : " << o.first() << std::endl;
    log() << "  first : " << o.first() << std::endl;
    log() << "  first : " << o.first() << std::endl;
    log() << "  first : " << o.first() << std::endl;
    log() << "   last : " << o.last() << std::endl;
  }
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "  count : " << o.count() << std::endl;
  }
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "    sum : " << o.sum() << std::endl;
  }
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "average : " << o.average() << std::endl;
  }
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "    min : " << o.min() << std::endl;
  }
  {
    auto o = observables::range(1, 10) | blocking();
    log() << "    max : " << o.max() << std::endl;
  }

  log() << "string array" << std::endl;
  auto arr = {std::string("a"), std::string("b")};
  {
    auto o = observables::iterate(arr) | blocking();
    log() << "  first : " << o.first() << std::endl;
  }
  {
    auto o = observables::iterate(arr) | blocking();
    log() << "   last : " << o.last() << std::endl;
  }
  {
    auto o = observables::iterate(arr) | blocking();
    log() << "  count : " << o.count() << std::endl;
  }
  
  {
    log() << "emit error" << std::endl;
    auto o = observable<>::create<int>([](subscriber<int> s){
      s.on_next(1);
      s.on_error(std::make_exception_ptr(std::exception()));
    }) | blocking();
    try{
      log() << "  count : " << o.count() << std::endl;
    }
    catch(...){
      log() << "exception" << std::endl;
    }
  }
  
  {
    log() << "emit empty" << std::endl;
    auto o = observable<>::create<int>([](subscriber<int> s){
      s.on_completed();
    }) | blocking();
    try{
      log() << "  count : " << o.count() << std::endl;
      log() << "  first : " << o.first() << std::endl;
    }
    catch(...){
      log() << "exception" << std::endl;
    }
  }

  wait(1000);

  log() << "test_blocking -- end" << std::endl << std::endl;
}