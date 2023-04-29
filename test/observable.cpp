#include <another-rxcpp/observable.h>

#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/operators/map.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_observable() {
  log() << "test_observable -- begin" << std::endl;


  {
    log() << "#1" << std::endl;
    auto ob = ovalue(123)
    | flat_map([](int x){
      log() << x << std::endl;
      return ovalue(std::string("abc"))
      | map([](std::string x){
        log() << x << std::endl;
        return 456;
      });
    });
    doSubscribe(ob);
  }

  {
    auto ob = ovalue(1)
    | flat_map([](int64_t x){
      log() << x << std::endl;
      return ovalue(std::string("abc"), 500);
    })
    | flat_map([](std::string x){
      log() << x << std::endl;
      return ovalue(5);
    })
    | flat_map([](int x){
      log() << x << std::endl;
      return ovalue(x + 1, 500);
    })
    | flat_map([](int x){
      log() << x << std::endl;
      return ovalue(x + 1);
    });
    {
      log() << "#3 wait until is_subscribed() == true" << std::endl;
      auto x = doSubscribe(ob);
      while(x.is_subscribed()) {}
    }
    {
      log() << "#4 unsubscribe after 5000ms" << std::endl;
      auto x = doSubscribe(ob);
      wait(5000);
      x.unsubscribe();
    }
    {
      log() << "#5 unsubscribe after 700ms" << std::endl;
      auto x = doSubscribe(ob);
      wait(700);
      x.unsubscribe();
    }
  }

  log() << "test_observable -- end" << std::endl << std::endl;
}
