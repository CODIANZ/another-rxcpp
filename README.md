# another-rxcpp

`RxCpp` とは異なる実装の `ReactiveX` です。 

## 背景

弊社で開発しているアプリでは `RxCpp` を多用しております。この `RxCpp` はとても素晴らしいライブラリであり、非同期処理におけるパラダイムシフトを感じます。
しかし、この `RxCpp` なのですが、数百から数千にも及ぶ `Observable` のメソッドチェーンを実装すると、コンパイル時間はもとより、デバッガさえも起動しなくなるレベルになってしまいます。

その結果、デバッグシンボルを除去したり、デバッグ実行でも最適化を実施したりすることで対応しています。
しかし、ただでさえデバッグが難しい `ReactiveX` が更に追跡が難しくなり、そしてコンパイル時間も最適化により増大し、徐々に開発環境が悪化していき現在に至ります。

色々と改善方法を模索したのですが万策尽きた感があるため、いっそのこと `RxCpp` とは異なる実装で `ReactiveX` を実装しようと考え `another-rxcpp` を作成しました。

## 特徴

* C++14 以降に対応しました。
* `Observable` の `Operators` をメンバメソッドから分離しました。
* `Observable` に上流の `Type` を持たせないことで、 `as_dynamic()` を不要としました。これにより、コンパイル時間とデバッガへの負荷を軽減できれば嬉しいです。
* `Take` などのオペレータをスレッドセーフにしました。
* `C++` のマルチスレッディングを利用した機能を追加しました。（`subscription::unsubscribe_notice()` など）

## デメリット・懸念点

* 未実装機能が圧倒的に多いです。
* `Operators` をメンバメソッドから分離したことで型推論が効かず、`flat_map` や `map` オペレータなどで多くの場合 `auto` が使えません。

## 目標・やること

* 未実装機能の中でも重要な実装を早めに行いたいです。（`Scheduler`や`Subject`など）
* `RxCpp` と `another-rxcpp` を混在する仕組みがあると嬉しい。
* `JavaScript` の `Promise` を `C++` で実装した [jpromise](https://github.com/CODIANZ/jpromise) との相互運用ができるようにしたい。
* バージョン番号管理をちゃんとしたい。
* `noexcept` を適宜追加する。

## 実装状況

* observable
* subscription
* just
* range
* flat_map
* map
* take

## 使用方法

`another-rxcpp` はヘッダファイルのみで構成されています。このリポジトリの `include` ディレクトリをインクルード検索に追加してください。

下記のインクルードファイルを指定してください。（そのうち、まとめる予定です）
```cpp
#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
```

`namespace` は `another_rxcpp` になります。

## 使用例

### common.h

```cpp
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
```

### observable, map, flat_map

```cpp
void test_observable() {
  log() << "test_observable -- begin" << std::endl;

  {
    log() << "#1" << std::endl;
    auto ob = ovalue(123)
    | flat_map([](int&& x){
      log() << x << std::endl;
      return ovalue(std::string("abc"))
      | map([](std::string&& x){
        log() << x << std::endl;
        return 456;
      });
    });
    doSubscribe(ob);
  }

  {
    auto ob = ovalue(1)
    | flat_map([](int&& x){
      log() << x << std::endl;
      return ovalue(std::string("abc"), 500);
    })
    | flat_map([](std::string&& x){
      log() << x << std::endl;
      return ovalue(5);
    })
    | flat_map([](int&& x){
      log() << x << std::endl;
      return ovalue(x + 1, 500);
    })
    | flat_map([](int&& x){
      log() << x << std::endl;
      return ovalue(x + 1);
    });
    {
      log() << "#2 wait with notify_on_unsubscribe()" << std::endl;
      auto x = doSubscribe(ob);
      std::mutex mtx;
      std::unique_lock<std::mutex> lock(mtx);
      x.unsubscribe_notice()->wait(lock, [x](){ return x.is_subscribed(); });
    }
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
```

### just

```cpp
void test_just() {
  log() << "test_just -- begin" << std::endl;

  doSubscribe(observables::just(1));
  doSubscribe(observables::just(std::string("abc")));
  doSubscribe(observables::just(1.23));
  
  log() << "test_just -- end" << std::endl << std::endl;
}
```

### range

```cpp
void test_range() {
  log() << "test_range -- begin" << std::endl;

  log() << "1 - 10" << std::endl;
  doSubscribe(observables::range(1, 10));

  log() << "-10 - 5" << std::endl;
  doSubscribe(observables::range(-10, 5));

  log() << "test_range -- end" << std::endl << std::endl;
}
```

### take

```cpp
void test_take() {
  log() << "test_take -- begin" << std::endl;

  auto o = observables::range(1, 100);

  doSubscribe(o | take(0));
  doSubscribe(o | take(1));
  doSubscribe(o | take(5));

  auto x = doSubscribe(
    o
    | flat_map([](int&& x){
      return ovalue(x, 100);
    })
    | take(10)
  );
  while(x.is_subscribed()) {}

  log() << "test_take -- end" << std::endl << std::endl;
}
```