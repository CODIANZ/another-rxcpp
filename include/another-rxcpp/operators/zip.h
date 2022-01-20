#if !defined(__another_rxcpp_h_zip__)
#define __another_rxcpp_h_zip__

#include "../observable.h"
#include "../internal/tools/util.h"
#include "../internal/tools/any_sp_keeper.h"
#include "../schedulers.h"
#include <algorithm>
#include <vector>

namespace another_rxcpp {
namespace operators {

namespace zip_internal {

  /** tuple<OB1::value_type, OB2::value_type...> */
  template <typename ...> struct result_type_impl;

  template <typename ...TPARGS, typename OB, typename ...ARGS>
    struct result_type_impl<std::tuple<TPARGS...>, OB, ARGS...> :
      result_type_impl<std::tuple<TPARGS..., typename OB::value_type>, ARGS...> {};

  template <typename ...TPARGS>
    struct result_type_impl<std::tuple<TPARGS...>> {
      using type = std::tuple<TPARGS...>;
    };

  template <typename ...ARGS> struct result_type {
    using type = typename result_type_impl<std::tuple<>, ARGS...>::type;
  };


  /**
   * create values queue 
   * tuple< shared_ptr<queue<OB1::value_type>>, shared_ptr<queue<OB1::value_type>>, ...>
   **/
  template <typename TPL>
    auto create_values_queue_internal(TPL tpl)
  { 
    return tpl;
  }

  template <typename TPL, typename OB, typename ...ARGS>
    auto create_values_queue_internal(TPL tpl, OB /* ob */, ARGS...args)
  { 
    return create_values_queue_internal(
      std::tuple_cat(
        tpl,
        std::make_tuple(
          std::make_shared<std::queue<typename OB::value_type>>()
        )
      ),
      args...);
  }

  template <typename ...ARGS>
    auto create_values_queue(ARGS...args)
  {
    return create_values_queue_internal(std::tuple<>(), args...);
  }


  /* members for synchronization */
  struct sync {
    using sp = std::shared_ptr<sync>;
    std::mutex              mtx_;
    std::condition_variable cond_;
    std::exception_ptr      err_ = nullptr;
    std::size_t             completed_ = 0;
  };


  /** subscribe each observable */
  template <std::size_t N, typename VALQ>
    void subscribe_impl(sync::sp, std::vector<subscription>& sbsc, const VALQ& valq)
  { /** nothing to do */ }

  template <std::size_t N, typename VALQ, typename OB, typename... ARGS>
    void subscribe_impl(sync::sp sync, std::vector<subscription>& sbsc, const VALQ& valq, OB ob, ARGS...args)
  {
    subscribe_impl<N + 1>(sync, sbsc, valq, args...);

    auto values = std::get<N>(valq);
    auto src = internal::private_access::observable::create_source(ob);

    sbsc.push_back(
      src->subscribe({
        .on_next = [values, sync](auto x){
          {
            std::lock_guard<std::mutex> lock(sync->mtx_);
            values->push(std::move(x));
            sync->cond_.notify_one();
          }
        },
        .on_error = [sync](std::exception_ptr err){
          {
            std::lock_guard<std::mutex> lock(sync->mtx_);
            if(sync->err_ != nullptr) sync->err_ = err;
            sync->cond_.notify_one();
          }
        },
        .on_completed = [sync](){
          {
            std::lock_guard<std::mutex> lock(sync->mtx_);
            sync->completed_++;
            sync->cond_.notify_one();
          }
        }
      })
    );
  }

  template <typename VALQ, typename... ARGS>
    void subscribe(sync::sp sync, std::vector<subscription>& sbsc, const VALQ& valq, ARGS...args)
  {
    subscribe_impl<0>(sync, sbsc, valq, args...);
  }


  /* check all values are ready */
  template <std::size_t N, typename VALQ>
    bool ready_values_impl(const VALQ& valq)
  {
    return true;
  }

  template <std::size_t N, typename VALQ, typename OB, typename... ARGS>
    bool ready_values_impl(const VALQ& valq, OB /* ob */, ARGS...args)
  {
    return std::get<N>(valq)->size() > 0 && ready_values_impl<N + 1>(valq, args...);
  }

  template <typename VALQ, typename... ARGS>
    bool ready_values(const VALQ& valq, ARGS...args)
  {
    return ready_values_impl<0>(valq, args...);
  }


  /* get all the first values & pop */
  template <std::size_t N, typename VALQ, typename TPL>
    auto get_values_impl(TPL tpl, const VALQ& valq)
  {
    return tpl;
  }

  template <std::size_t N, typename VALQ, typename TPL, typename OB, typename... ARGS>
    auto get_values_impl(TPL tpl, const VALQ& valq, OB /* ob */, ARGS...args)
  {
    auto values = std::get<N>(valq);
    auto v = values->front();
    values->pop();
    return get_values_impl<N + 1>(
      std::tuple_cat(tpl, std::make_tuple(std::move(v))),
      valq,
      args...
    );
  }

  template <typename VALQ, typename... ARGS>
    auto get_values(const VALQ& valq, ARGS...args)
  {
    return get_values_impl<0>(std::tuple<>(), valq, args...);
  }

  /** http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3802.pdf */
  template <typename F, typename Tuple, size_t... I>
    decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
  {
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
  }

  template <typename F, typename Tuple>
    decltype(auto) apply(F&& f, Tuple&& t)
  {
    using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
    return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
  }

  template <typename...ARGS>
  auto zip_main(ARGS...args) {
    return [args...](auto src) {
      using rtype = typename zip_internal::result_type<decltype(src), ARGS...>::type;
      return observable<>::create<rtype>([src, args...](subscriber<rtype> s) {
        auto valq = zip_internal::create_values_queue(src, args...);
        auto sync = std::make_shared<zip_internal::sync>();
        auto sbsc = std::vector<subscription>();
        zip_internal::subscribe(sync, sbsc, valq, src, args...);

        while(s.is_subscribed()){
          std::unique_lock<std::mutex> lock(sync->mtx_);
          sync->cond_.wait(lock, [&](){
            return sbsc.size() == sync->completed_ || sync->err_ != nullptr || zip_internal::ready_values(valq, src, args...);
          });
          if(sync->err_){
            s.on_error(sync->err_);
            break;
          }
          else if(zip_internal::ready_values(valq, src, args...)){
            s.on_next(zip_internal::get_values(valq, src, args...));
          }
          else {
            if(sbsc.size() == sync->completed_){
              s.on_completed();
              break;
            }
          }
        }
        std::for_each(sbsc.begin(), sbsc.end(), [](auto it){ it.unsubscribe(); });
      });
    };
  }
} /* zip_internal */

template <typename X, typename...ARGS, std::enable_if_t<is_observable<X>::value, bool> = true>
  auto zip(X x, ARGS...args)
{
  return zip_internal::zip_main(std::forward<X>(x), std::forward<ARGS>(args)...);
}

template <typename X, typename...ARGS, std::enable_if_t<!is_observable<X>::value, bool> = true>
  auto zip(X x, ARGS...args)
{
  return [x, args...](auto src){
    using FRET = internal::lambda_invoke_result_t<X>;
    return observable<>::create<FRET>([x, args..., src](subscriber<FRET> s){
      auto ups = internal::private_access::observable::create_source(
        zip_internal::zip_main(args...)(src)
      );
      internal::private_access::subscriber::add_upstream(s, ups);
      ups->subscribe({
        .on_next = [s, x](auto r){
          s.on_next(zip_internal::apply(x, r));
        },
        .on_error = [s](std::exception_ptr err){
          s.on_error(err);
        },
        .on_completed = [s](){
          s.on_completed();
        }
      });
    });
  };
}



} /* namespace operators */
} /* namespace another_rxcpp */

#endif /* !defined(__another_rxcpp_h_zip__) */