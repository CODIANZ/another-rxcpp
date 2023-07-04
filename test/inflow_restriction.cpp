#include <another-rxcpp/utils/inflow_restriction.h>

#include <another-rxcpp/observable.h>
#include <another-rxcpp/observables/just.h>
#include <another-rxcpp/observables/iterate.h>
#include <another-rxcpp/operators/tap.h>
#include <another-rxcpp/operators/delay.h>
#include <another-rxcpp/operators/map.h>
#include <another-rxcpp/operators/flat_map.h>
#include <another-rxcpp/schedulers/new_thread_scheduler.h>
#include <another-rxcpp/utils/unit.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::schedulers;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::utils;

void test_inflow_restriction() {
  log() << "test_inflow_restriction -- begin" << std::endl;

  enum class result { success, failure };
  struct long_api {
    std::shared_ptr<std::mutex> mtx_ = std::make_shared<std::mutex>();
    std::shared_ptr<int> count_ = std::make_shared<int>(0);
    observable<result> call() {
      auto mtx = mtx_;
      auto count = count_;
      return observables::just(unit{}, new_thread_scheduler())
      | tap([mtx, count](unit){
        std::lock_guard<std::mutex> lock(*mtx);
        (*count)++;
        const int x = (*count)++;
        std::cout << std::this_thread::get_id() << " : enter #" << x << std::endl;
      })
      | delay(std::chrono::seconds(1), new_thread_scheduler())
      | map([](unit){
        return result::success;
      })
      | tap([mtx, count](result){
        std::lock_guard<std::mutex> lock(*mtx);
        const int x = (*count)--;
        std::cout << std::this_thread::get_id() << " : leave #" << x << std::endl;
      });
    }
  };

  {
    log() << "without inflow_restriction" << std::endl;
    auto mtx = std::make_shared<std::mutex>();
    auto list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto api = std::make_shared<long_api>();

    auto sbsc = (
      observables::iterate(list)
      | flat_map([api](int n){
        return api->call()
        | map([n](result){
          return n;
        });
      })
    )
    .subscribe({
      [mtx](const int& x){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "next " << x << std::endl;
      },
      [mtx](std::exception_ptr){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "error " << std::endl;
      },
      [mtx](){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  {
    log() << "use inflow_restriction" << std::endl;
    auto mtx = std::make_shared<std::mutex>();
    auto list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto api = std::make_shared<long_api>();
    auto ifr = std::make_shared<inflow_restriction<4>>();

    auto sbsc = (
      observables::iterate(list)
      | flat_map([ifr, api](int n){
        return ifr->enter(api->call())
        | map([n](result){
          return n;
        });
      })
    )
    .subscribe({
      [mtx](const int& x){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "next " << x << std::endl;
      },
      [mtx](std::exception_ptr){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "error " << std::endl;
      },
      [mtx](){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  {
    log() << "use inflow_restriction + unsubscribe" << std::endl;
    auto mtx = std::make_shared<std::mutex>();
    auto list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto api = std::make_shared<long_api>();
    auto ifr = std::make_shared<inflow_restriction<4>>();

    auto sbsc = (
      observables::iterate(list)
      | flat_map([ifr, api](int n){
        return ifr->enter(api->call())
        | tap([n](auto){
          log() << "tap: " << n << std::endl;
        })
        | map([n](result){
          return n;
        });
      })
    )
    .take(5)
    .subscribe({
      [mtx](const int& x){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "next " << x << std::endl;
      },
      [mtx](std::exception_ptr){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "error " << std::endl;
      },
      [mtx](){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  log() << "test_inflow_restriction -- end" << std::endl << std::endl;
}