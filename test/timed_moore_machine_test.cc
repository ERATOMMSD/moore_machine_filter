#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
// (setq flycheck-clang-language-standard "c++14")
#include "../src/timed_moore_machine.hh"
#include "../src/add_counter.hh"

BOOST_AUTO_TEST_SUITE(TimedMooreMachineTest)

BOOST_AUTO_TEST_CASE( constructNexts1 )
{
  std::array<TAState*, 4> states;
  for(auto &s: states) {
    s = new TAState();
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

  DRTAStateWithCounter* s = new DRTAStateWithCounter();
  std::unordered_map<DRTAStateWithCounter*, std::unordered_multimap<TAState*, DBM>> toOldStates;
  toOldStates[s].emplace(std::make_pair(states[0], D));
  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nexts;

  constructNexts(s, toOldStates, nexts);

  BOOST_REQUIRE_EQUAL(nexts['a'].size(), 3);
  std::array<TAState*, 3> expectedStates = {{states[1], states[2], states[3]}};
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

class LinearlizeNexts {
public:
  LinearlizeNexts(){}
  void construct(const std::size_t stateSize, 
                 const std::vector<Bounds> &lowerBounds, 
                 const std::vector<Bounds> &upperBounds) {
    states.resize(stateSize);
    for(auto &s: states) {
      s = new TAState();
    }
    DBMs.resize(stateSize-1);
    for (auto &D: DBMs) {
      D.resize(2, 2);
    }
    nexts['a'].reserve(stateSize-1);
    for (std::size_t i = 0; i < stateSize-1; i++) {
      DBM D = DBM::zero(2);
      D.value = DBMs[i];
      nexts['a'].emplace_back(states.at(i+1), D, lowerBounds.at(i), upperBounds.at(i));
    }
  }
  void test (const std::size_t expectedStates,
             const std::vector<std::size_t> &expectedStatesNums,
             const std::vector<Bounds> expectedBounds) {
    std::unordered_multimap<TAState*, DBM> initConfs;
    std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<TAState*, DBM>, Bounds>>> nextLineared;

    linearlizeNexts(nexts, initConfs, nextLineared);
  
    BOOST_CHECK_EQUAL(nextLineared['a'].size(), expectedStates);
    for (std::size_t i = 0; i < expectedStates; i++) {
      BOOST_CHECK_EQUAL(nextLineared['a'][i].first.size(), expectedStatesNums[i]);
      BOOST_CHECK_EQUAL(nextLineared['a'][i].second, expectedBounds[i]);
    }
  }
  std::vector<TAState*> states;  
  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nexts;
  std::vector<Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic>> DBMs;
};

BOOST_AUTO_TEST_CASE( linearlizeNexts1 )
{
  std::array<TAState*, 4> states;
  for(auto &s: states) {
    s = new TAState();
  }

  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nexts;
  std::array<Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic>, 3> DBMs;
  for (auto &D: DBMs) {
    D.resize(2, 2);
  }
  DBMs[0] << Bounds{0, true}, Bounds{-4, false}, Bounds{6, false}, Bounds{0, true};
  DBMs[1] << Bounds{0, true}, Bounds{0, true}, Bounds{0, true}, Bounds{0, true};
  DBMs[2] << Bounds{0, true}, Bounds{-7, false}, Bounds{8, false}, Bounds{0, true};
  std::array<Bounds, 3> lowerBounds = {{Bounds{0, false}, Bounds{0, false}, Bounds{-2, false}}};
  std::array<Bounds, 3> upperBounds = {{Bounds{3, false}, Bounds{4, false}, Bounds{5, false}}};

  nexts['a'].reserve(3);
  for (int i = 0; i < 3; i++) {
    DBM D = DBM::zero(2);
    D.value = DBMs[i];
    nexts['a'].emplace_back(states.at(i+1), D, lowerBounds.at(i), upperBounds.at(i));
  }


  std::unordered_multimap<TAState*, DBM> initConfs;
  std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<TAState*, DBM>, Bounds>>> nextLineared;

  linearlizeNexts(nexts, initConfs, nextLineared);
  
  // NOTE: the size of states is not 6 but 5 because we prohibit t = 0.
  BOOST_CHECK_EQUAL(nextLineared['a'].size(), 5);
  std::array<std::size_t, 5> expectedStatesNums = {{2, 3, 2, 1, 0}};
  std::array<Bounds, 5> expectedBounds = {{Bounds{2, true}, Bounds{3, false}, Bounds{4, false}, Bounds{5, false}, Bounds{std::numeric_limits<double>::infinity(), false}}};
  for (int i = 0; i < 5; i++) {
    BOOST_CHECK_EQUAL(nextLineared['a'][i].first.size(), expectedStatesNums[i]);
    BOOST_CHECK_EQUAL(nextLineared['a'][i].second, expectedBounds[i]);
  }
}

BOOST_FIXTURE_TEST_CASE( linearlizeNexts2, LinearlizeNexts )
{
  std::vector<Bounds> lowerBounds = {Bounds{-0, false}, Bounds{-2, false}, Bounds{-3, false}};
  std::vector<Bounds> upperBounds = {Bounds{1, false}, Bounds{3, false}, Bounds{4, false}};
  construct(4, lowerBounds, upperBounds);

  constexpr const std::size_t expectedNextStates = 6;
  std::vector<std::size_t> expectedStatesNums = {1, 0, 1, 0, 1, 0};
  std::vector<Bounds> expectedBounds = {Bounds{1, false}, Bounds{2, true}, Bounds{3, false}, Bounds{3, true}, Bounds{4, false}, Bounds{std::numeric_limits<double>::infinity(), false}};

  test(expectedNextStates, expectedStatesNums, expectedBounds);
}

BOOST_AUTO_TEST_CASE( linearlizeNexts3 )
{
  std::array<TAState*, 5> states;
  for(auto &s: states) {
    s = new TAState();
  }

  boost::unordered_map<unsigned char, std::vector<std::tuple<TAState*, DBM, Bounds, Bounds>>> nexts;
  constexpr const std::size_t targetSize = 4;
  std::array<Eigen::Matrix<Bounds, Eigen::Dynamic, Eigen::Dynamic>, targetSize> DBMs;
  for (auto &D: DBMs) {
    D.resize(2, 2);
  }
  std::array<Bounds, targetSize> lowerBounds = {{Bounds{-0, false}, Bounds{-0, false}, Bounds{-2, true}, Bounds{-3, true}}};
  std::array<Bounds, targetSize> upperBounds = {{Bounds{1, true}, Bounds{3, true}, Bounds{3, true}, Bounds{4, true}}};

  constexpr const std::size_t expectedNextStates = 6;
  std::array<std::size_t, expectedNextStates> expectedStatesNums = {{2, 1, 2, 3, 1, 0}};
  std::array<Bounds, expectedNextStates> expectedBounds = {{Bounds{1, true}, Bounds{2, false}, Bounds{3, false}, Bounds{3, true}, Bounds{4, true}, Bounds{std::numeric_limits<double>::infinity(), false}}};

  nexts['a'].reserve(targetSize);
  for (std::size_t i = 0; i < targetSize; i++) {
    DBM D = DBM::zero(2);
    D.value = DBMs[i];
    nexts['a'].emplace_back(states.at(i+1), D, lowerBounds.at(i), upperBounds.at(i));
  }


  std::unordered_multimap<TAState*, DBM> initConfs;
  std::unordered_map<unsigned char, std::vector<std::pair<std::unordered_multimap<TAState*, DBM>, Bounds>>> nextLineared;

  linearlizeNexts(nexts, initConfs, nextLineared);
  
  BOOST_CHECK_EQUAL(nextLineared['a'].size(), expectedNextStates);
  for (std::size_t i = 0; i < expectedNextStates; i++) {
    BOOST_CHECK_EQUAL(nextLineared['a'][i].first.size(), expectedStatesNums[i]);
    BOOST_CHECK_EQUAL(nextLineared['a'][i].second, expectedBounds[i]);
  }
}

BOOST_FIXTURE_TEST_CASE( linearlizeNextsUpperOpenAndClosed, LinearlizeNexts )
{
  std::vector<Bounds> lowerBounds = {{Bounds{-2, false}, Bounds{-3, false}}};
  std::vector<Bounds> upperBounds = {{Bounds{5, true}, Bounds{5, false}}};
  construct(3, lowerBounds, upperBounds);

  constexpr const std::size_t expectedNextStates = 5;
  std::vector<std::size_t> expectedStatesNums = {{0, 1, 2, 1, 0}};
  std::vector<Bounds> expectedBounds = {{Bounds{2, true}, Bounds{3, true}, Bounds{5, false}, Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), false}}};

  test(expectedNextStates, expectedStatesNums, expectedBounds);
}

BOOST_FIXTURE_TEST_CASE( linearlizeNextsUpperOpenAndOpen, LinearlizeNexts )
{
  std::vector<Bounds> lowerBounds = {{Bounds{-2, false}, Bounds{-3, false}}};
  std::vector<Bounds> upperBounds = {{Bounds{5, false}, Bounds{5, false}}};
  construct(3, lowerBounds, upperBounds);

  constexpr const std::size_t expectedNextStates = 4;
  std::vector<std::size_t> expectedStatesNums = {{0, 1, 2, 0}};
  std::vector<Bounds> expectedBounds = {{Bounds{2, true}, Bounds{3, true}, Bounds{5, false}, Bounds{std::numeric_limits<double>::infinity(), false}}};

  test(expectedNextStates, expectedStatesNums, expectedBounds);
}

BOOST_FIXTURE_TEST_CASE( linearlizeNextsLowerOpenAndClosed, LinearlizeNexts )
{
  std::vector<Bounds> lowerBounds = {{Bounds{-2, true}, Bounds{-2, false}}};
  std::vector<Bounds> upperBounds = {{Bounds{5, true}, Bounds{4, false}}};
  construct(3, lowerBounds, upperBounds);

  constexpr const std::size_t expectedNextStates = 5;
  std::vector<std::size_t> expectedStatesNums = {{0, 1, 2, 1, 0}};
  std::vector<Bounds> expectedBounds = {{Bounds{2, false}, Bounds{2, true}, Bounds{4, false}, Bounds{5, true}, Bounds{std::numeric_limits<double>::infinity(), false}}};

  test(expectedNextStates, expectedStatesNums, expectedBounds);
}

BOOST_FIXTURE_TEST_CASE( linearlizeNextsEverythingAt3, LinearlizeNexts )
{
  /*
    Input Bounds
    (1,3), (2,3], (3,5), [3,4)
   */
  std::vector<Bounds> lowerBounds = {Bounds{-1, false}, Bounds{-2, false}, Bounds{-3, false}, Bounds{-3, true}};
  std::vector<Bounds> upperBounds = {Bounds{3, false}, Bounds{3, true}, Bounds{5, false}, Bounds{4, false}};
  construct(5, lowerBounds, upperBounds);

  constexpr const std::size_t expectedNextStates = 7;
  std::vector<std::size_t> expectedStatesNums = {0, 1, 2, 2, 2, 1, 0};
  std::vector<Bounds> expectedBounds = {Bounds{1, true}, Bounds{2, true}, Bounds{3, false}, Bounds{3, true}, Bounds{4, false}, Bounds{5, false}, Bounds{std::numeric_limits<double>::infinity(), false}};

  test(expectedNextStates, expectedStatesNums, expectedBounds);
}

BOOST_AUTO_TEST_CASE(filterTest)
{
  using Alphabet = std::pair<unsigned char, double>;

  TimedAutomaton from;
  constexpr std::size_t bufferSize = 2;
  TAWithCounter<bufferSize> to;
  MooreMachine<bufferSize, Alphabet, DRTAStateWithCounter> filter;

  from.states.reserve(3);

  for (int i = 0; i < 3; i++) {
    from.states.push_back(new TAState());
  }

  std::array<bool, 3> match = {{false, false, true}};
  std::array<TAState*, 2> next_weak = {{from.states[1], from.states[2]}};
  std::array<std::vector<ClockVariables>, 2> next_reset = {{{}, {}}};
  std::array<std::vector<Constraint>, 2> next_guard = {{{TimedAutomaton::X(0) < 1}, {TimedAutomaton::X(0) < 1}}};
  std::array<unsigned char, 2> next_char = {{'a', 'b'}};

  for (int i = 0; i < 2; i++) {
    from.states[i]->isMatch = match[i];
    from.states[i]->nextMap[next_char[i]] = {{next_weak[i], next_reset[i], next_guard[i]}};
  }
  from.states[2]->isMatch = match[2];

  from.initialStates = {from.states[0]};
  from.states[0]->isMatch = false;
  from.states[1]->isMatch = false;
  from.states[2]->isMatch = true;

  from.maxConstraints = {1};

  toAutomatonWithCounter(from, to);

  BOOST_REQUIRE_EQUAL(to.states.size(), 3);
  BOOST_TEST(!to.states[0]->isMatch);
  BOOST_TEST(!to.states[1]->isMatch);
  BOOST_TEST( to.states[2]->isMatch);
  BOOST_CHECK_EQUAL(to.states[0]->nextMap['a'].size(), 1);
  BOOST_CHECK_EQUAL(to.states[0]->nextMap['b'].size(), 0);
  BOOST_CHECK_EQUAL(to.states[1]->nextMap['a'].size(), 0);
  BOOST_CHECK_EQUAL(to.states[1]->nextMap['b'].size(), 1);
  BOOST_TEST(to.states[2]->nextMap.empty());
  BOOST_CHECK_EQUAL(to.counter[to.states[0]], 0);
  BOOST_CHECK_EQUAL(to.counter[to.states[1]], 1);
  BOOST_CHECK_EQUAL(to.counter[to.states[2]], 2);

  toTimedMooreMachine(to, Bounds{1, true}, 1, filter);

  std::vector<Alphabet> inputTimedWord = {{'a', 0.2},
                                          {'b', 0.6},
                                          {'a', 0.1},
                                          {'b', 1.2},
                                          {'a', 0.7},
                                          maskChar<Alphabet>,
                                          maskChar<Alphabet>};
  
  std::vector<Alphabet> outputTimedWord;
  outputTimedWord.reserve(inputTimedWord.size());
  std::vector<Alphabet> outputTimedWordExpected = {maskChar<Alphabet>,
                                                   maskChar<Alphabet>,
                                                   {'a', 0.2},
                                                   {'b', 0.6},
                                                   mask(std::pair<unsigned char, double>{'a', 0.1}),
                                                   mask(std::pair<unsigned char, double>{'b', 1.2}),
                                                   mask(std::pair<unsigned char, double>{'a', 0.7})};
  for (const auto &c: inputTimedWord) {
    outputTimedWord.push_back(filter.feed(c));
  }
  BOOST_CHECK_EQUAL_COLLECTIONS(outputTimedWord.begin(), outputTimedWord.end(),
                                outputTimedWordExpected.begin(), outputTimedWordExpected.end());
}

BOOST_AUTO_TEST_CASE(constructLoopTest)
{
  using Alphabet = std::pair<unsigned char, double>;

  TimedAutomaton from;
  constexpr std::size_t bufferSize = 2;
  TAWithCounter<bufferSize> to;
  MooreMachine<bufferSize, Alphabet, DRTAStateWithCounter> filter;

  constexpr std::size_t fromStateSize = 3;

  from.states.reserve(fromStateSize);

  for (std::size_t i = 0; i < fromStateSize; i++) {
    from.states.push_back(new TAState());
  }

  std::array<bool, fromStateSize> match = {{false, false, true}};
  for (std::size_t i = 0; i < fromStateSize; i++) {
    from.states[i]->isMatch = match[i];
  }

  from.states[0]->nextMap['a'] = {{from.states[1], {}, {TimedAutomaton::X(0) < 1}}};

  std::array<TAState*, 2> next_weak = {{from.states[1], from.states[2]}};
  std::array<std::vector<ClockVariables>, 2> next_reset = {{{}, {}}};
  std::array<std::vector<Constraint>, 2> next_guard = {{{}, {}}};
  std::array<unsigned char, 2> next_char = {{'a', 'b'}};

  for (int i = 0; i < 2; i++) {
    from.states[1]->nextMap[next_char[i]] = {{next_weak[i], next_reset[i], next_guard[i]}};
  }

  from.initialStates = {from.states[0]};
  from.maxConstraints = {1};

  toAutomatonWithCounter(from, to);

  BOOST_REQUIRE_EQUAL(to.states.size(), 5);

  toTimedMooreMachine(to, Bounds{1, true}, 1, filter);
}

BOOST_AUTO_TEST_SUITE_END()
