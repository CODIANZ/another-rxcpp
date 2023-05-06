#if defined(SUPPORTS_OPERATORS_IN_OBSERVABLE) || defined(SUPPORTS_RXCPP_COMPATIBLE)
  #include <another-rxcpp/observable.h>
#else
  #include <another-rxcpp/operators/zip.h>
  #include <another-rxcpp/observable.h>
#endif

#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators/map.h>

#include "common.h"
#include <sstream>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

static auto emitter(thread_group threads) {
  return observable<>::create<int>([threads](subscriber<int> s){
    threads.push([s](){
      for(int i = 0; i < 10; i++){
        if(!s.is_subscribed()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.on_next(i);
      }
      s.on_completed();
    });
  });
}

static auto emitter2(thread_group threads) {
  return emitter(threads)
  | map([](int x){
    return double(x + 11.1);
  });
}

static auto emitter3(thread_group threads) {
  return emitter(threads)
  | map([](int x){
    std::stringstream ss;
    ss << "s" << x;
    return ss.str();
  });
}

void test_zip() {

  log() << "test_zip -- begin" << std::endl;

  {
    auto o = observables::range(0, 9)
    | zip(observables::range(10, 19), observables::range(20, 29))
    | map([](std::tuple<int, int, int> tp){
      log() << "[0] " << std::get<0>(tp) << std::endl;
      log() << "[1] " << std::get<1>(tp) << std::endl;
      log() << "[2] " << std::get<2>(tp) << std::endl;
      return std::get<0>(tp);
    });
    auto x = doSubscribe(o);
    while(x.is_subscribed()) {}
  }

  {
    thread_group threads;
    auto o = emitter(threads)
    | zip(emitter2(threads), emitter3(threads))
    | map([](std::tuple<int, double, std::string> tp){
      log() << "[0] " << std::get<0>(tp) << std::endl;
      log() << "[1] " << std::get<1>(tp) << std::endl;
      log() << "[2] " << std::get<2>(tp) << std::endl;
      return std::get<0>(tp);
    });
    auto x = doSubscribe(o);
    while(x.is_subscribed()) {}
    threads.join_all();
  }

  {
    thread_group threads;
    auto o = emitter(threads)
    | zip([](int a, double b, std::string c){
      std::stringstream ss;
      ss << a << ", " << b << ", " << c;
      return ss.str();
    }, emitter2(threads), emitter3(threads));

    auto x = doSubscribe(o);
    while(x.is_subscribed()) {}
    threads.join_all();
  }

  log() << "test_zip -- end" << std::endl << std::endl;
}