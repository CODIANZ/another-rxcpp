#include <another-rxcpp/observable.h>
#include <another-rxcpp/subjects/subject.h>
#include <another-rxcpp/subjects/behavior.h>
#include <another-rxcpp/utils/ready_set_go.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

static observable<int> _subject(thread_group threads) {
  auto t = [](){
    subjects::subject<int> sbj;
    return std::make_tuple(sbj.as_subscriber(), sbj.as_observable());
  }();
  auto s = std::get<0>(t);
  auto o = std::get<1>(t);
  return utils::ready_set_go([s, threads](){
    threads.push([s](){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      s.on_next(1);
      s.on_completed();
    });
  }, o);
}

static observable<int> _behavior(thread_group threads) {
  auto t = [](){
    subjects::behavior<int> sbj(99);
    return std::make_tuple(sbj.as_subscriber(), sbj.as_observable());
  }();
  auto s = std::get<0>(t);
  auto o = std::get<1>(t);
  return utils::ready_set_go([s, threads](){
    threads.push([s](){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      s.on_next(1);
      s.on_completed();
    });
  }, o);
}

static observable<int> _subject_error() {
  auto t = [](){
    subjects::subject<int> sbj;
    return std::make_tuple(sbj.as_subscriber(), sbj.as_observable());
  }();
  auto s = std::get<0>(t);
  auto o = std::get<1>(t);
  s.on_error(std::make_exception_ptr(std::exception()));
  return o;
}

void test_case_2() {
  log() << "test_case_2 -- begin" << std::endl;

  thread_group threads;

  {
    log() << "_subject" << std::endl;
    auto x = doSubscribe(_subject(threads));
    while(x.is_subscribed()) {}
    threads.join_all();
  }

  {
    log() << "_behavior" << std::endl;
    auto x = doSubscribe(_behavior(threads));
    while(x.is_subscribed()) {}
    threads.join_all();
  }

  {
    log() << "_subject_error" << std::endl;
    auto x = doSubscribe(_subject_error());
    while(x.is_subscribed()) {}
  }

  log() << "test_case_2 -- end" << std::endl << std::endl;
}