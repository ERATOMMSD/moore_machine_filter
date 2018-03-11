#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/moore_machine.hh"

BOOST_AUTO_TEST_SUITE(MooreMachineTest)

BOOST_AUTO_TEST_CASE( toMooreMachine1 )
{
  constexpr std::size_t bufferSize = 2;
  NFAWithCounter<bufferSize> from;
  MooreMachine<bufferSize> to;

  from.states.reserve(4);

  for (int i = 0; i < 4; i++) {
    from.states.push_back(std::make_shared<NFAState>());
  }

  std::array<bool, 4> match = {{false, false, true, false}};

  for (int i = 0; i < 3; i++) {
    from.states[i]->isMatch = match[i];
    from.counter[from.states[i]] = 0;
  }

  from.states[0]->next['a'] = {from.states[1], from.states[3]};
  from.states[1]->next['b'] = {from.states[1], from.states[3]};
  from.states[2]->next['a'] = {from.states[3]};
  from.states[3]->next['a'] = {from.states[1], from.states[2]};

  from.initStates = {from.states[0], from.states[1], from.states[2]};
  std::sort(from.initStates.begin(), from.initStates.end());

  toMooreMachine(from, to);

  BOOST_REQUIRE_EQUAL(to.states.size(), 2);
  BOOST_REQUIRE_EQUAL(to.initialStates.size(), 1);

  std::array<bool, 2> matchResult = {{true, true}};
  std::array<std::size_t, 2> counterResult = {{0, 0}};

  for (int i = 0; i < 2; i++) {
    BOOST_CHECK_EQUAL(to.states.at(i)->isMatch, matchResult.at(i));
    BOOST_CHECK_EQUAL(to.counter[to.states.at(i)], counterResult.at(i));
  }

  BOOST_CHECK_EQUAL(to.states.at(0)->next('a'), to.states.at(1));
  BOOST_CHECK_EQUAL(to.states.at(0)->next('b'), to.states.at(1));
  BOOST_CHECK_EQUAL(to.states.at(1)->next('a'), to.states.at(1));
  BOOST_CHECK_EQUAL(to.states.at(1)->next('b'), to.states.at(1));
}

BOOST_AUTO_TEST_SUITE_END()
