#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/add_counter.hh"

BOOST_AUTO_TEST_SUITE(AddCounterTest)

BOOST_AUTO_TEST_CASE( example1 )
{
  NFA from;
  constexpr std::size_t bufferSize = 2;
  NFAWithCounter<bufferSize> to;

  from.states.reserve(3);

  for (int i = 0; i < 3; i++) {
    from.states.push_back(std::make_shared<NFAState>());
  }

  std::array<bool, 3> match = {{false, false, true}};
  std::array<std::weak_ptr<NFAState>, 3> next_weak = {{from.states[1], from.states[2], from.states[0]}};

  for (int i = 0; i < 3; i++) {
    from.states[i]->isMatch = match[i];
    from.states[i]->nextMap['a'] = {next_weak[i]};
  }

  from.initialStates = {from.states[0]};
  from.states[0]->isMatch = false;
  from.states[1]->isMatch = false;
  from.states[2]->isMatch = true;

  toNFAWithCounter(from, to);

  BOOST_REQUIRE_EQUAL(to.states.size(), 7);
  BOOST_REQUIRE_EQUAL(to.initialStates.size(), 1);

  std::array<bool, 7> matchResult = {{false, false, true, false, false, true, false}};
  std::array<std::weak_ptr<NFAState>, 7> next_weakResult = 
    {{to.states[1], to.states[2], to.states[3],
      to.states[4], to.states[5], to.states[6], to.states[1]}};
  std::array<std::size_t, 7> counterResult = {{0, 1, 2, 1, 2, 1, 2}};

  for (int i = 0; i < 7; i++) {
    BOOST_CHECK_EQUAL(to.states.at(i)->isMatch, matchResult.at(i));
    BOOST_CHECK_EQUAL(to.states.at(i)->nextMap['a'].size(), 1);
    BOOST_CHECK_EQUAL(to.states.at(i)->nextMap['a'][0].lock(), next_weakResult.at(i).lock());
    BOOST_CHECK_EQUAL(to.counter[to.states.at(i)], counterResult.at(i));
  }
}

BOOST_AUTO_TEST_SUITE_END()
