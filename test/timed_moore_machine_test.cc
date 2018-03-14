#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
// (setq flycheck-clang-language-standard "c++14")
#include "../src/timed_moore_machine.hh"
#include "../src/add_counter.hh"

BOOST_AUTO_TEST_SUITE(MooreMachineTest)

BOOST_AUTO_TEST_CASE( constructNexts1 )
{
  std::array<std::shared_ptr<TAState>, 4> states;
  for(auto &s: states) {
    s = std::make_shared<TAState>();
  }
  states[0]->nextMap['a'] = {{states[1], {}, {TimedAutomaton::X(0) > 4, TimedAutomaton::X(0) < 6}},
                             {states[2], {0}, {TimedAutomaton::X(0) > 5, TimedAutomaton::X(0) < 7}},
                             {states[3], {}, {TimedAutomaton::X(0) > 7, TimedAutomaton::X(0) < 8}}};

  DBM D = DBM::zero(2);
  D.value.fill(Bounds(std::numeric_limits<double>::infinity(), false));
  D.value.diagonal().fill(Bounds(0, true));
  D.value.row(0).fill(Bounds(0, true));
  D.value(1, 0) = Bounds{5, false};
  D.value(0, 1) = Bounds{-3, false};

  std::shared_ptr<DRTAState> s = std::make_shared<DRTAState>();
  std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> toOldStates;
  toOldStates[s].emplace(std::make_pair(states[0], D));
  boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> nexts;

  constructNexts(s, toOldStates, nexts);

  BOOST_REQUIRE_EQUAL(nexts['a'].size(), 3);
  std::array<std::shared_ptr<TAState>, 3> expectedStates = {{states[1], states[2], states[3]}};
  std::array<Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic>, 3> expectedDBMs;
  for (auto &D: expectedDBMs) {
      D.resize(2, 2);
  }
  expectedDBMs[0] << Bounds{0, true}, Bounds{-4, false}, Bounds{6, false}, Bounds{0, true};
  expectedDBMs[1] << Bounds{0, true}, Bounds{0, true}, Bounds{0, true}, Bounds{0, true};
  expectedDBMs[2] << Bounds{0, true}, Bounds{-7, false}, Bounds{8, false}, Bounds{0, true};
  std::array<Bounds, 3> expectedLowerBounds = {{Bounds{0, false}, Bounds{0, false}, Bounds{-2, false}}};
  std::array<Bounds, 3> expectedUpperBounds = {{Bounds{3, false}, Bounds{4, false}, Bounds{5, false}}};

  for (int i = 0; i < 3; i++) {
    std::get<1>(nexts['a'][i]).value.diagonal().fill(Bounds{0, true});
    BOOST_CHECK_EQUAL(std::get<0>(nexts['a'][i]), expectedStates[i]);
    BOOST_TEST((std::get<1>(nexts['a'][i]).value == expectedDBMs[i]));
    BOOST_TEST((std::get<2>(nexts['a'][i]) == expectedLowerBounds[i]));
    BOOST_TEST((std::get<3>(nexts['a'][i]) == expectedUpperBounds[i]));
  }
}

BOOST_AUTO_TEST_CASE( linearlizeNexts1 )
{
//   std::array<std::shared_ptr<TAState>, 4> states;
//   for(auto &s: states) {
//     s = std::make_shared<TAState>();
//   }
//   states[0]->nextMap['a'] = {{states[1], {}, {TimedAutomaton::X(0) > 4, TimedAutomaton::X(0) < 6}},
//                              {states[2], {}, {TimedAutomaton::X(0) > 5, TimedAutomaton::X(0) < 7}},
//                              {states[3], {}, {TimedAutomaton::X(0) > 7, TimedAutomaton::X(0) < 8}}};

//   DBM D = DBM::zero(2);
//   D.value.fill(Bounds(std::numeric_limits<double>::infinity(), false));
//   D.value.diagonal().fill(Bounds(0, true));
//   D.value.row(0).fill(Bounds(0, true));
//   D.value(1, 0) = Bounds{5, false};
//   D.value(0, 1) = Bounds{-3, false};

//   std::shared_ptr<DRTAState> s = std::make_shared<DRTAState>();
//   std::unordered_map<std::shared_ptr<DRTAState>, std::unordered_multimap<std::shared_ptr<TAState>, DBM>> toOldStates;
//   toOldStates[s].emplace(std::make_pair(states[0], D));
//   boost::unordered_map<unsigned char, std::vector<std::tuple<std::shared_ptr<TAState>, DBM, Bounds, Bounds>>> nexts;

//   constructNexts(s, toOldStates, nexts);

//   BOOST_REQUIRE_EQUAL(nexts['a'].size(), 3);
//   std::array<std::shared_ptr<TAState>, 3> expectedStates = {{states[1], states[2], states[3]}};
//   std::array<Bounds, 3> expectedLowerBounds = {{Bounds{0, false}, Bounds{0, false}, Bounds{-2, false}}};
//   std::array<Bounds, 3> expectedUpperBounds = {{Bounds{3, false}, Bounds{4, false}, Bounds{5, false}}};

//   for (int i = 0; i < 3; i++) {
//     BOOST_CHECK_EQUAL(std::get<0>(nexts['a'][i]), expectedStates[i]);
//     BOOST_TEST((std::get<2>(nexts['a'][i]) == expectedLowerBounds[i]));
//     BOOST_TEST((std::get<3>(nexts['a'][i]) == expectedUpperBounds[i]));
//   }
}

BOOST_AUTO_TEST_SUITE_END()
