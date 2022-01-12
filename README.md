# another-rxcpp

`RxCpp` とは異なる実装の `ReactiveX` です。

## 背景

弊社で開発しているアプリでは `ReactiveX` を多用しております。この `ReactiveX` はとても素晴らしいライブラリであり、当時は非同期処理におけるパラダイムシフトを実感しました。
しかし、この `ReactiveX` の `RxCpp` なのですが、数百から数千にも及ぶ `Observable` のメソッドチェーンを実装すると、コンパイル時間はもとより、デバッガさえも起動しなくなるレベルになってしまいます。

その結果、デバッグシンボルを除去したり、デバッグ実行でも最適化を実施したりすることで対応しています。
しかし、ただでさえデバッグが難しい `ReactiveX` が更に追跡が難しくなり、そしてコンパイル時間も最適化により増大し、徐々に開発環境が悪化していき現在に至ります。

色々と改善方法を模索したのですが万策尽きた感があるため、いっそのこと `RxCpp` とは異なる実装で `ReactiveX` を実装しようと考え `another-rxcpp` を作成しました。

## RxCpp との違い

背景と説明が被りますが、`RxCpp` の多くのオペレータは第２テンプレートパラメータに上流の `observable` の型を丸ごと抱える構造になっています。小規模のプログラムでは問題は無いのですが、大量にメソッドチェーンを行う場合、第２テンプレートパラメータが徐々に増大していき、下記の問題が発生します。

* 具体化される `observable` テンプレートの具体化される個数が増加し、コンパイル速度の低下、バイナリの増加が著しいです。
* 同様に、デバッグシンボルの増加が著しく、デバッガがクラッシュします。
* 上記の問題は `as_dynamic()` にて多少は改善されるますが、それでも限界は訪れます。

そこで `another-rxcpp` では上流の `observable` の型を不要とする改善を行い、大規模プロジェクトでも `rxcpp` が利用できるようになります。

## 特徴

* C++14 以降に対応しました。
* `Observable` に上流の `Type` を持たせないことで、 `as_dynamic()` を不要としました。これにより、コンパイル時間とデバッガへの負荷を軽減します。（弊社のプロダクトでは`RxCpp`よりもコンパイル時間が半分程度、デバッグシンボルは10分の1程度になりました。）
* `Observable` の `Operators` をメンバメソッドから分離しました。
* `Take` などの内部で判定が必要なオペレータをスレッドセーフにしました。（ブロッキングしないバージョンも準備する予定です）
* `C++` のマルチスレッディングを利用した機能を追加しました。（`subscription::unsubscribe_notice()` など）
* `SUPPORTS_OPERATORS_IN_OBSERVABLE` を定義することで、従来のメソッドチェーンをサポートします。（型推論が効くメリットがありますが、observableの型ごとの関数インスタンス量がが増えるデメリットがあります）
* `SUPPORTS_RXCPP_COMPATIBLE` を定義することで、`subscribe()` が `RxCpp` に準じた引数を使用することができます。また、`as_dynamic()` も実装されます。（`as_dynamic()`は本当になにも行わず `observable` をコピーして返却するだけです）

## 注意事項

`rxcpp` は多くのコントリビュータの手によってメンテナンスされております。対して、 `another-rxcpp` は弊社で開発して、現状、弊社のプロジェクトで動作確認を進めている最中のプロジェクトです。したがって、中小規模のプロジェクトであれば `rxcpp` を使用されることを強くお勧めします。

なお、`another-rxcpp` で不具合やご要望等ございましたら `issues` に記載して頂ければ、可能な限り改修を行いたいと考えております。


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
* last
* map
* merge
* observe_on
* on_error_resume_next
* publish
* retry
* subscribe_on
* skip_while
* take_last
* take_until
* take
* tap
* timeout

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

## 使用方法

`another-rxcpp` はヘッダファイルのみで構成されています。このリポジトリの `include` ディレクトリをインクルード検索に追加してください。

下記のインクルードファイルを指定してください。（そのうち、まとめる予定です）
```cpp
#include <another-rxcpp/rx.h>
```

`namespace` は `another_rxcpp` になります。

`RxCpp` と互換性が必要な場合は下記のようにします。

```cpp
#define SUPPORTS_OPERATORS_IN_OBSERVABLE
#define SUPPORTS_RXCPP_COMPATIBLE
#include <another-rxcpp/rx.h>
```

定義はコンパイルオプションで設定すると良いでしょう。

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
    .on_next = [](auto x) {
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
