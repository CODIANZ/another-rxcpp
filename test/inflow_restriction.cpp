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
    std::mutex mtx;
    int count_ = 0;
    observable<result> call() {
      return observable<>::just(unit{}, observe_on_new_thread())
      .tap([=](unit){
        std::lock_guard<std::mutex> lock(mtx);
        const int x = count_++; 
        std::cout << std::this_thread::get_id() << " : enter #" << x << std::endl;
      })
      .delay(std::chrono::seconds(1), observe_on_new_thread())
      .map([=](unit){
        return result::success;
      })
      .tap([=](result){
        std::lock_guard<std::mutex> lock(mtx);
        const int x = count_--;
        std::cout << std::this_thread::get_id() << " : leave #" << x << std::endl;
      });
    }
  };

  {
    log() << "without inflow_restriction" << std::endl;
    std::mutex mtx;
    auto list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto api = std::make_shared<long_api>();

    auto sbsc = (
      observable<>::iterate(list)
      | flat_map([=](int n){
        return api->call()
        | map([=](result){
          return n;
        });
      })
    )
    .subscribe({
      .on_next = [&](int x){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "next " << x << std::endl;
      },
      .on_error = [&](std::exception_ptr){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "error " << std::endl;
      },
      .on_completed = [&](){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  {
    log() << "use inflow_restriction" << std::endl;
    std::mutex mtx;
    auto list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    auto api = std::make_shared<long_api>();
    auto ifr = std::make_shared<inflow_restriction<4>>();

    auto sbsc = (
      observable<>::iterate(list)
      | flat_map([=](int n){
        return ifr->enter(api->call())
        | map([=](result){
          return n;
        });
      })
    )
    .subscribe({
      .on_next = [&](int x){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "next " << x << std::endl;
      },
      .on_error = [&](std::exception_ptr){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "error " << std::endl;
      },
      .on_completed = [&](){
        std::lock_guard<std::mutex> lock(mtx);
        log() << "completed " << std::endl;
      }
    });

    while(sbsc.is_subscribed()) {}    
  }

  log() << "test_inflow_restriction -- end" << std::endl << std::endl;
}