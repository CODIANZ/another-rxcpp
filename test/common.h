#if !defined(__h_common__)
#define __h_common__

#include <functional>
#include <thread>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <another-rxcpp/observable.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include <another-rxcpp/operators/subscribe.h>

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

class thread_group {
  mutable std::shared_ptr<std::vector<std::thread>> threads_;

public:
  thread_group() noexcept :
    threads_(std::make_shared<std::vector<std::thread>>()) {}

  template <typename F> void push(F&& f) const noexcept {
    threads_->push_back(std::thread(std::forward<F>(f)));
  }

  void join_all() const noexcept {
    std::for_each(threads_->begin(), threads_->end(), [](auto& t){ t.join(); });
    threads_->clear();
  }
};

inline void setTimeout(std::function<void()> f, int x) {
  observables::just(0)
  .observe_on(schedulers::new_thread_scheduler())
  .delay(std::chrono::milliseconds(x))
  .subscribe([f](auto){
    f();
  });
}

inline std::ostream& log() {
  return std::cout << "(" << std::hex << std::this_thread::get_id() << ") " << std::dec;
}

inline void wait(int ms) {
  log() << "wait " << ms << "ms" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  log() << "awake" << std::endl;
}


template <typename T>
auto ovalue(T&& value, int delay = 0) {
  if(delay == 0){
    return observables::just(std::forward<T>(value));
  }
  else{
    return observables::just(std::forward<T>(value))
    .observe_on(schedulers::new_thread_scheduler())
    .delay(std::chrono::milliseconds(delay));
  }
}

inline auto interval_range(int from, int to, int msec, thread_group threads) {
  return observable<>::create<int>([from, to, msec, threads](subscriber<int> s){
    threads.push([from, to, msec, s]{
      for(int i = from; i <= to; i++){
        if(!s.is_subscribed()){
          log() << "interval_range break" << std::endl;
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
        if(!s.is_subscribed()){
          log() << "interval_range break" << std::endl;
          return;
        }
        log() << "interval_range emit " << i << std::endl;
        s.on_next(i);
      }
      log() << "interval_range complete" << std::endl;
      s.on_completed();
    });
  });
}

template <typename T> auto doSubscribe(T&& source) {
  using TT = typename another_rxcpp::internal::strip_const_reference<T>::type;
  log() << "doSubscribe" << std::endl;
  using value_type = typename TT::value_type;
  return source
  | subscribe([](const value_type& x) {
  // .subscribe([](value_type&& x) {
    log() << "  [on_next] " << x << std::endl;
  }, [](std::exception_ptr err) {
    std::string w = [err](){
      try{ std::rethrow_exception(err); }
      catch(std::exception& e){
        return e.what();
      }
      catch(...){
        return "unknown";
      }
    }();
    log() << "  [on_error] " << w << std::endl;
  }, []() {
    log() << "  [on_completed] " << std::endl;
  });
}

#endif /* !defined(__h_common__) */