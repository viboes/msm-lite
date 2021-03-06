//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "boost/msm-lite.hpp"
#include <cassert>
#include <typeinfo>
#include <iostream>

namespace msm = boost::msm::lite;

struct e1 {};
struct e2 {};
struct e3 {};
struct e4 {};

auto guard1 = [] {
  std::cout << "guard1" << std::endl;
  return true;
};

auto guard2 = [](int i) {
  assert(42 == i);
  std::cout << "guard2" << std::endl;
  return false;
};

auto action1 = [](auto e) { std::cout << "action1: " << typeid(e).name() << std::endl; };
struct action2 {
  void operator()(int i) {
    assert(42 == i);
    std::cout << "action2" << std::endl;
  }
};

struct actions_guards {
  auto configure() const noexcept {
    using namespace msm;
    // clang-format off
    return make_transition_table(
       *"idle"_s + event<e1> = "s1"_s
      , "s1"_s + event<e2> [ guard1 ] / action1 = "s2"_s
      , "s2"_s + event<e3> [ guard1 && ![] { return false;} ] / (action1, action2{}) = "s3"_s
      , "s3"_s + event<e4> [ !guard1 || guard2 ] / (action1, [] { std::cout << "action3" << std::endl; }) = "s4"_s
      , "s3"_s + event<e4> [ guard1 ] / ([] { std::cout << "action4" << std::endl; }) = X
    );
    // clang-format on
  }
};

int main() {
  msm::sm<actions_guards> sm{42};
  assert(sm.process_event(e1{}));
  assert(sm.process_event(e2{}));
  assert(sm.process_event(e3{}));
  assert(sm.process_event(e4{}));
  assert(sm.is(msm::X));
}
