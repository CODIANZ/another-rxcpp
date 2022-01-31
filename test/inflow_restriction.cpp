#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/utils.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::schedulers;
using namespace another_rxcpp::operators;
using namespace another_rxcpp::utils;

void test_inflow_restriction() {
  log() << "test_inflow_restriction -- begin" << std::endl;

  enum class result { success, failure };
  struct long_api {
    std::mutex mtx_;
    int count_;
    observable<result> call() {
      return observables::just(unit{}, new_thread_scheduler())
      | tap([=](unit){
        std::lock_guard<std::mutex> lock(mtx_);
        const int x = count_++; 
        std::cout << std::this_thread::get_id() << " : enter #" << x << std::endl;
      })
      | delay(std::chrono::seconds(1), new_thread_scheduler())
      | map([=](unit){
        return result::success;
      })
      | tap([=](result){
        std::lock_guard<std::mutex> lock(mtx_);
        const int x = count_--;
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
      | flat_map([=](int n){
        return api->call()
        | map([=](result){
          return n;
        });
      })
    )
    .subscribe({
      [=](int&& x){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "next " << x << std::endl;
      },
      [=](std::exception_ptr){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "error " << std::endl;
      },
      [=](){
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
      | flat_map([=](int n){
        return ifr->enter(api->call())
        | map([=](result){
          return n;
        });
      })
    )
    .subscribe({
      [=](int&& x){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "next " << x << std::endl;
      },
      [=](std::exception_ptr){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "error " << std::endl;
      },
      [=](){
        std::lock_guard<std::mutex> lock(*mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  log() << "test_inflow_restriction -- end" << std::endl << std::endl;
}