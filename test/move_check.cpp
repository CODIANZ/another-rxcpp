#include <another-rxcpp/observable.h>
#include <another-rxcpp/operators.h>
#include <another-rxcpp/observables.h>
#include "common.h"

using namespace another_rxcpp;
using namespace another_rxcpp::operators;

struct A {
    static int ctor_default;
    static int ctor_copy;
    static int ctor_move;
    static int dtor;
    A() {
      ctor_default++;
      log() << "A::ctor default" << std::endl;
    }
    A(const A&) {
      ctor_copy++;
      log() << "A::ctor copy" << std::endl; 
    }
    A(A&&) {
      ctor_move++;
      log() << "A::ctor move" << std::endl;
    }
    ~A() {
      dtor++;
      log() << "A::dtor" << std::endl;
    }
    operator int () const { return 1; }
    
    static void reset() {
      ctor_default = 0;
      ctor_copy = 0;
      ctor_move = 0;
      dtor = 0;
    }

    static void dump() {
      log() 
        << "d: " << ctor_default
        << ", c: " << ctor_copy
        << ", m: " << ctor_move
        << ", +-: " << (ctor_default + ctor_copy + ctor_move - dtor)
        << std::endl;
    }
};

int A::ctor_default;
int A::ctor_copy;
int A::ctor_move;
int A::dtor;


void move_check() {
  log() << "move_check -- begin" << std::endl;

  {
    log () << "** just **" << std::endl;
    A::reset();
    doSubscribe(observables::just(A()));
    A::dump();
  }


  {
    log () << "** just twice **" << std::endl;
    A::reset();
    {
      auto o = observables::just(A());
      doSubscribe(o);
      doSubscribe(o);
    }
    A::dump();
  }

  auto o = observable<>::create<A>([](subscriber<A> s){
    s.on_next(A());
    s.on_completed();
  });

  {
    log () << "** plain **" << std::endl;
    A::reset();
    doSubscribe(o);
    A::dump();
  }

  {
    log () << "** flat_map **" << std::endl;
    A::reset();
    doSubscribe(o | flat_map([&](const A&){
      return o;
    }));
    A::dump();
  }

  {
    log () << "** map **" << std::endl;
    A::reset();
    doSubscribe(o | map([](const A&){
      return A();
    }));
    A::dump();
  }

  log() << "move_check -- end" << std::endl << std::endl;
}