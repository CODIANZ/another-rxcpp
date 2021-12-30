#if !defined(__h_common__)
#define __h_common__

#include <functional>
#include <thread>
#include <iostream>
#include <another-rxcpp/observable.h>

using namespace another_rxcpp;

inline void setTimeout(std::function<void()> f, int x) {
  auto t = std::thread([f, x]{
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
    f();
  });
  t.detach();
}

inline std::ostream& log() {
  return std::cout << "(" << std::hex << std::this_thread::get_id() << ") " << std::dec;
}

inline void wait(int ms) {
  log() << "wait " << ms << "ms" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  log() << "awake" << std::endl;
}


template <typename T, typename TT = typename std::remove_const<typename std::remove_reference<T>::type>::type>
auto ovalue(T&& value, int delay = 0) -> observable<TT> {
  auto _value = std::forward<T>(value);
  return observable<>::create<TT>([_value, delay](subscriber<TT> s) {
    if(delay == 0){
      s.on_next(std::move(_value));
      s.on_completed();
    }
    else{
      setTimeout([s, _value]() {
        s.on_next(std::move(_value));
        s.on_completed();
      }, delay);
    }
  });
}

template <typename T> auto doSubscribe(T source) {
  log() << "doSubscribe" << std::endl;
  return source.subscribe({
    .on_next = [](auto&& x) {
      log() << "  [on_next] " << x << std::endl;
    },
    .on_error = [](std::exception_ptr err) {
      log() << "  [on_error] " << std::endl;
    },
    .on_completed = []() {
      log() << "  [on_completed] " << std::endl;
    }
  });
}

#endif /* !defined(__h_common__) */