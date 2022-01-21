# another-rxcpp

`another-rxcpp` is a `Reactive Extensions` with a different implementation than `RxCpp`.

## Why not `RxCpp`?

`RxCpp` is a great library.

However, `RxCpp` has the only and biggest problem.
That is, the `Observable` of `RxCpp` has a structure that holds the entire type of the upstream `Observable` in the second template parameter.
This problem is not a concern for small projects, but for medium-sized projects and above, the development environment becomes quite poor as shown below.

* The generated binary size will be huge. (Because the number of embodied templates increases)
* The debug symbol becomes huge. (If it gets too big, the debugger will crash)
* Build time is not unusually long.

`RxCpp` uses `as_dynamic()` and `Observable` can be improved by clearing the second template parameter, but the effect is insignificant.

However, it is not practical to radically eliminate the `Observable` second template parameter of `RxCpp`.
So I created a new `Reactive Extensions` for `C++ `to solve these problems as `another-rxcpp`.


## Features

* Compile time is reduced.
* The binary size will be smaller.
* The smaller size of the debug symbol makes the debugger start faster.

### Details

* By not giving `Observable` an upstream `Type`, we no longer need `as_dynamic()`. This reduces compile time and load on the debugger. (In our product, the compile time is about half that of `RxCpp`, and the debug symbol is about 1/10.)
* Separated the `Operators` of the `Observable` from the member methods. (Although it also exists in the current `RxCPP`, this is the default in `another-rxcpp`)
* Made operators that need to be judged internally, such as `Take`, thread-safe. (We plan to prepare a version that does not block)
* Added the function using multithreading of `C++`. (For example, `subscription::unsubscribe_notice()`)
* Support traditional method chains by defining `SUPPORTS_OPERATORS_IN_OBSERVABLE`. (There is a merit that type inference works, but there is a demerit that the amount of function instances for each type of observable increases.)
* By defining `SUPPORTS_RXCPP_COMPATIBLE`,`subscribe()`can use arguments similar to`RxCpp`. It also implements `as_dynamic()`. (`as_dynamic()` does nothing, just copy and return `Observable`)


## Usage

`another-rxcpp` consists only of header files. Add the `include` directory of this repository to the header search path.

Specify the following include file.

```cpp
#include <another-rxcpp/rx.h>
```

`namespace` is `another_rxcpp`.

If compatibility with `RxCpp` is required, define as follows before including `rx.h`. (It should be included in the compile options.)

```cpp
#define SUPPORTS_OPERATORS_IN_OBSERVABLE
#define SUPPORTS_RXCPP_COMPATIBLE
#include <another-rxcpp/rx.h>
```


### Development environment

* C++14 and above

### Test environment

* Xcode 12.3
* Android Studio 4.3.0
* gcc 7.5.0
* clang 6.0.0


## Implementation status

### observable

* observable
* connectable observable
* blocking observable
* just
* never
* error
* empty
* range
* interval
* iterate

### operator

* amb
* blocking
* delay
* distinct_until_changed
* finally
* flat_map
* filter
* first
* last
* map
* merge
* observe_on
* on_error_resume_next
* publish
* retry
* skip_until
* skip_while
* subscribe_on
* take_last
* take_until
* take_while
* take
* tap
* timeout
* zip

### subject

* subject
* behavior subject

### scheduler

* scheduler
* default_scheduler
* new_thread_scheduler (observe_on_new_thread)
* async_scheduler

### util

* ready_set_go
* inflow_restriction
* sem
* unit
* something


## Examples

### common.h

```cpp
inline void setTimeout(std::function<void()> f, int x){
  auto t = std::thread([f, x]{
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
    f();
  });
  t.detach();
}

inline std::ostream& log(){
  return std::cout << "(" << std::hex << std::this_thread::get_id()<< ")" << std::dec;
}

inline void wait(int ms){
  log()<< "wait " << ms << "ms" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  log()<< "awake" << std::endl;
}

template <typename T, typename TT = typename std::remove_const<typename std::remove_reference<T>::type>::type>
auto ovalue(T value, int delay = 0)-> observable<TT> {
  auto _value = std::forward<T>(value);
  return observable<>::create<TT>([_value, delay](subscriber<TT> s){
    if(delay == 0){
      s.on_next(std::move(_value));
      s.on_completed();
    }
    else{
      setTimeout([s, _value](){
        s.on_next(std::move(_value));
        s.on_completed();
      }, delay);
    }
  });
}

template <typename T> auto doSubscribe(T source){
  log()<< "doSubscribe" << std::endl;
  return source.subscribe({
    .on_next = [](auto x){
      log()<< "  [on_next] " << x << std::endl;
    },
    .on_error = [](std::exception_ptr err){
      log()<< "  [on_error] " << std::endl;
    },
    .on_completed = [](){
      log()<< "  [on_completed] " << std::endl;
    }
  });
}
```

### observable, map, flat_map

```cpp
void test_observable(){
  log()<< "test_observable -- begin" << std::endl;

  {
    log()<< "#1" << std::endl;
    auto ob = ovalue(123)
    | flat_map([](int x){
      log()<< x << std::endl;
      return ovalue(std::string("abc"))
      | map([](std::string x){
        log()<< x << std::endl;
        return 456;
      });
    });
    doSubscribe(ob);
  }

  {
    auto ob = ovalue(1)
    | flat_map([](int x){
      log()<< x << std::endl;
      return ovalue(std::string("abc"), 500);
    })
    | flat_map([](std::string x){
      log()<< x << std::endl;
      return ovalue(5);
    })
    | flat_map([](int x){
      log()<< x << std::endl;
      return ovalue(x + 1, 500);
    })
    | flat_map([](int x){
      log()<< x << std::endl;
      return ovalue(x + 1);
    });
    {
      log()<< "#2 wait with notify_on_unsubscribe()" << std::endl;
      auto x = doSubscribe(ob);
      std::mutex mtx;
      std::unique_lock<std::mutex> lock(mtx);
      x.unsubscribe_notice()->wait(lock, [x](){ return x.is_subscribed(); });
    }
    {
      log()<< "#3 wait until is_subscribed()== true" << std::endl;
      auto x = doSubscribe(ob);
      while(x.is_subscribed()){}
    }
    {
      log()<< "#4 unsubscribe after 5000ms" << std::endl;
      auto x = doSubscribe(ob);
      wait(5000);
      x.unsubscribe();
    }
    {
      log()<< "#5 unsubscribe after 700ms" << std::endl;
      auto x = doSubscribe(ob);
      wait(700);
      x.unsubscribe();
    }
  }

  log()<< "test_observable -- end" << std::endl << std::endl;
}
```

### just

```cpp
void test_just(){
  log()<< "test_just -- begin" << std::endl;

  doSubscribe(observables::just(1));
  doSubscribe(observables::just(std::string("abc")));
  doSubscribe(observables::just(1.23));
  
  log()<< "test_just -- end" << std::endl << std::endl;
}
```

### range

```cpp
void test_range(){
  log()<< "test_range -- begin" << std::endl;

  log()<< "1 - 10" << std::endl;
  doSubscribe(observables::range(1, 10));

  log()<< "-10 - 5" << std::endl;
  doSubscribe(observables::range(-10, 5));

  log()<< "test_range -- end" << std::endl << std::endl;
}
```

### take

```cpp
void test_take(){
  log()<< "test_take -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | take(0));
  doSubscribe(o | take(1));
  doSubscribe(o | take(5));

  auto x = doSubscribe(
    o
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | take(10)
  );
  while(x.is_subscribed()){}

  log()<< "test_take -- end" << std::endl << std::endl;
}
```

### connectable observable

```cpp
void test_connectable(){
  log()<< "test_connectable -- begin" << std::endl;

  auto o = observable<>::create<int>([](subscriber<int> s){
    std::thread([s](){
      for(int i = 0; i < 100; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        s.on_next(i);
      }
      s.on_completed();
    }).detach();
  });

  auto oo = o | publish();
  auto s1 = doSubscribe(oo);
  auto s2 = doSubscribe(oo);
  auto s3 = doSubscribe(oo);

  wait(1000);
  oo.connect();

  wait(800);
  s1.unsubscribe();

  wait(800);
  s2.unsubscribe();

  auto s4 = doSubscribe(oo);

  wait(800);
  s3.unsubscribe();

  wait(3000);
  s4.unsubscribe();

  log()<< "test_connectable -- end" << std::endl << std::endl;
}
```

### subject

```cpp
void test_subject(){
  log()<< "test_subject -- begin" << std::endl;

  auto sbj = std::make_shared<subjects::subject<int>>();

  std::weak_ptr<subjects::subject<int>> weak_sbj = sbj;
  std::thread([weak_sbj]()mutable {
    for(int i = 0; i < 100; i++){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto p = weak_sbj.lock();
      if(p)p->as_subscriber().on_next(i);
    }
    auto p = weak_sbj.lock();
    if(p)p->as_subscriber().on_completed();
  }).detach();

  wait(2500);
  auto s1 = doSubscribe(sbj->as_observable());

  wait(1000);
  auto s2 = doSubscribe(sbj->as_observable());

  wait(1000);
  auto s3 = doSubscribe(sbj->as_observable());

  wait(500);
  s1.unsubscribe();

  wait(500);
  s2.unsubscribe();

  wait(500);
  s3.unsubscribe();

  wait(1000);
  auto s4 = doSubscribe(sbj->as_observable());

  wait(2000);

  sbj.reset();

  log()<< "test_subject -- end" << std::endl << std::endl;
}
```

### retry

```cpp
void test_retry(){
  log()<< "test_retry -- begin" << std::endl;

  auto counter = std::make_shared<int>(0);

  auto o = observables::range(0, 10)
  | flat_map([counter](int x){
    log()<< "value = " << x << std::endl;
    if(x == 3){
      (*counter)++;
      log()<< "counter = " << *counter << std::endl;
      if(*counter > 5){
        return observables::just(x);
      }
      else{
        log()<< "retry" << std::endl;
        return observables::error<int>(std::make_exception_ptr(std::exception()));
      }
    }
    else return observables::just(x);
  })
  | retry();

  auto x = doSubscribe(o);
  
  log()<< "test_retry -- end" << std::endl << std::endl;
}
```
