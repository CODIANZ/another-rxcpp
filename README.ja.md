# another-rxcpp

`another-rxcpp` は `RxCpp` とは異なる実装の `Reactive Extensions` です。

## なぜ `RxCpp` ではないのか？

`RxCpp` は素晴らしいライブラリです。

しかし、`RxCpp`には唯一にして最大の問題があります。
それは、`RxCpp` の `Observable` は第２テンプレートパラメータに上流の `Observable` の型を丸ごと抱える構造になっていることです。
この問題は小規模プロジェクトであれば気にならないのですが、中規模プロジェクト以上になると、開発環境は下記のようにかなり劣悪なものになります。

* 生成されるバイナリサイズが巨大になります。（テンプレートの具体化数が多くなるため）
* デバッグシンボルが巨大になります。（あまりに巨大になるとデバッガがクラッシュします）
* ビルド時間が尋常ではない。

`RxCpp` では `as_dynamic()` を使用して、`Observable` のは第２テンプレートパラメータを消去することで改善されますが、効果は微々たるものです。

しかし、`RxCpp` の `Observable` の第２テンプレートパラメータを根本的に消去するのは現実的ではありません。
そこで、`another-rxcpp`としてこれらの問題を解決しようと新たに`C++`用の`Reactive Extensions`を作成しました。


## 特徴

* コンパイル時間が短縮されます。
* バイナリサイズが小さくなります。
* デバッグシンボルのサイズが小さくなることで、デバッガの起動が高速になります。

### 詳細

* `Observable` に上流の `Type` を持たせないことで、 `as_dynamic()` を不要としました。これにより、コンパイル時間とデバッガへの負荷を軽減します。（弊社のプロダクトでは`RxCpp`よりもコンパイル時間が半分程度、デバッグシンボルは10分の1程度になりました。）
* `Observable` の `Operators` をメンバメソッドから分離しました。（現行の`RxCPP`にも存在しますが、`another-rxcpp`ではこれがデフォルトです）
* `Take` などの内部で判定が必要なオペレータをスレッドセーフにしました。（ブロッキングしないバージョンも準備する予定です）
* `C++` のマルチスレッディングを利用した機能を追加しました。（`subscription::unsubscribe_notice()` など）
* `SUPPORTS_OPERATORS_IN_OBSERVABLE` を定義することで、従来のメソッドチェーンをサポートします。（型推論が効くメリットがありますが、observableの型ごとの関数インスタンス量がが増えるデメリットがあります）
* `SUPPORTS_RXCPP_COMPATIBLE` を定義することで、`subscribe()` が `RxCpp` に準じた引数を使用することができます。また、`as_dynamic()` も実装されます。（`as_dynamic()`は何も行わず `Observable` をコピーして返却するだけです）


## 使用方法

`another-rxcpp` はヘッダファイルのみで構成されています。このリポジトリの `include` ディレクトリをインクルード検索に追加してください。

下記のインクルードファイルを指定してください。
```cpp
#include <another-rxcpp/rx.h>
```

`namespace` は `another_rxcpp` になります。

`RxCpp` と互換性が必要な場合は`rx.h`をインクルードする前に下記のように定義をします。（コンパイルオプションに入れると良いでしょう）

```cpp
#define SUPPORTS_OPERATORS_IN_OBSERVABLE
#define SUPPORTS_RXCPP_COMPATIBLE
#include <another-rxcpp/rx.h>
```


### 動作環境

* C++14以上

### 確認環境

* Xcode 12.3
* Android Studio 4.3.0
* gcc 7.5.0
* clang 6.0.0


## 実装状況

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
auto ovalue(T value, int delay = 0) -> observable<TT> {
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
    [](auto x) {
      log() << "  [on_next] " << x << std::endl;
    },
    [](std::exception_ptr err) {
      log() << "  [on_error] " << std::endl;
    },
    []() {
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
    | flat_map([](int x){
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
    | flat_map([](int x){
      return ovalue(x, 100);
    })
    | take(10)
  );
  while(x.is_subscribed()) {}

  log() << "test_take -- end" << std::endl << std::endl;
}
```

### connectable observable

```cpp
void test_connectable() {
  log() << "test_connectable -- begin" << std::endl;

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

  log() << "test_connectable -- end" << std::endl << std::endl;
}
```

### subject

```cpp
void test_subject() {
  log() << "test_subject -- begin" << std::endl;

  auto sbj = std::make_shared<subjects::subject<int>>();

  std::weak_ptr<subjects::subject<int>> weak_sbj = sbj;
  std::thread([weak_sbj]() mutable {
    for(int i = 0; i < 100; i++){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      auto p = weak_sbj.lock();
      if(p) p->as_subscriber().on_next(i);
    }
    auto p = weak_sbj.lock();
    if(p) p->as_subscriber().on_completed();
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

  log() << "test_subject -- end" << std::endl << std::endl;
}
```

### retry

```cpp
void test_retry() {
  log() << "test_retry -- begin" << std::endl;

  auto counter = std::make_shared<int>(0);

  auto o = observables::range(0, 10)
  | flat_map([counter](int x){
    log() << "value = " << x << std::endl;
    if(x == 3){
      (*counter)++;
      log() << "counter = " << *counter << std::endl;
      if(*counter > 5){
        return observables::just(x);
      }
      else{
        log() << "retry" << std::endl;
        return observables::error<int>(std::make_exception_ptr(std::exception()));
      }
    }
    else return observables::just(x);
  })
  | retry();

  auto x = doSubscribe(o);
  
  log() << "test_retry -- end" << std::endl << std::endl;
}
```
