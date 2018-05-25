#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/automata.hh"

BOOST_AUTO_TEST_SUITE(AutomataTest)

BOOST_AUTO_TEST_CASE( DRTAStateNext )
{
  std::array<DRTAState*, 4> states;
  for (auto& s: states) {
    s = new DRTAState();
  }

  states[0]->nextMap['a'] = {
    {{2, true}, states[1]},
    {{4, false}, states[2]},
    {{std::numeric_limits<double>::infinity(), false}, states[3]},
  };

  BOOST_CHECK_EQUAL(states[0]->next({'a', 1.0}), states[1]);
  BOOST_CHECK_EQUAL(states[0]->next({'a', 2.0}), states[1]);
  BOOST_CHECK_EQUAL(states[0]->next({'a', 3.0}), states[2]);
  BOOST_CHECK_EQUAL(states[0]->next({'a', 3.5}), states[2]);
  BOOST_CHECK_EQUAL(states[0]->next({'a', 4.0}), states[3]);
  BOOST_CHECK_EQUAL(states[0]->next({'a', 40.0}), states[3]);
}

BOOST_AUTO_TEST_SUITE_END()
