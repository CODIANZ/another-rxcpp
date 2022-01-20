#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include <another-rxcpp/subjects.h>
#include "common.h"
#include <sstream>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

void test_zip() {

  log() << "test_zip -- begin" << std::endl;

  auto emitter = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i < 10; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });

  auto emitter2 = emitter
  | map([](int x){
    return double(x + 11.1);
  });

  auto emitter3 = emitter
  | map([](int x){
    std::stringstream ss;
    ss << "s" << x;
    return ss.str();
  });

  {
    auto o = emitter
    | zip(emitter2, emitter3)
    | map([](std::tuple<int, double, std::string> tp){
      log() << "[0] " << std::get<0>(tp) << std::endl;
      log() << "[1] " << std::get<1>(tp) << std::endl;
      log() << "[2] " << std::get<2>(tp) << std::endl;
      return std::get<0>(tp);
    });
    auto x = doSubscribe(o);
    while(x.is_subscribed()) {}
  }

  {
    auto o = emitter
    | zip([](int a, double b, std::string c){
      std::stringstream ss;
      ss << a << ", " << b << ", " << c;
      return ss.str();
    }, emitter2, emitter3);

    auto x = doSubscribe(o);
    while(x.is_subscribed()) {}
  }

  log() << "test_zip -- end" << std::endl << std::endl;
}