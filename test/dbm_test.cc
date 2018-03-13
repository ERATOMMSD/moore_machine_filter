#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/dbm.hh"
#include "../src/automata.hh"

BOOST_AUTO_TEST_SUITE(BDMTest)

BOOST_AUTO_TEST_CASE( findDurationTest )
{
  DBM orig, guard;
  orig = DBM::zero(3);
  guard = DBM::zero(3);

  // 1 \le x \le 2
  // 3 \le y \le 4
  orig.value << \
    Bounds(0, true), Bounds(-1, true), Bounds(-3,true), \
    Bounds(2, true), Bounds(0, true), Bounds(std::numeric_limits<double>::infinity(), false), \
    Bounds(4, true), Bounds(std::numeric_limits<double>::infinity(), false), Bounds(0, true);

  // 7 \le x \le 8
  // 9 \le y \le 10
  guard.value << \
    Bounds(0, true), Bounds(-7, true), Bounds(-9,true), \
    Bounds(8, true), Bounds(0, true), Bounds(std::numeric_limits<double>::infinity(), false), \
    Bounds(10, true), Bounds(std::numeric_limits<double>::infinity(), false), Bounds(0, true);

  Bounds lower, upper;
  orig.findDuration(guard, lower, upper);

  BOOST_CHECK_EQUAL(lower.first, -5);
  BOOST_CHECK_EQUAL(lower.second, true);
  BOOST_CHECK_EQUAL(upper.first, 7);
  BOOST_CHECK_EQUAL(upper.second, true);
}

BOOST_AUTO_TEST_CASE( findDurationTest2 )
{
  DBM orig, guard;
  orig = DBM::zero(2);
  guard = DBM::zero(2);

  // 3 < x < 5
  orig.value << Bounds(0, true), Bounds(-3, false), Bounds(5, false), Bounds(0, true);

  // 4 < x < 6
  guard.value << Bounds(0, true), Bounds(-4, false), Bounds(6, false), Bounds(0, true);

  Bounds lower, upper;
  orig.findDuration(guard, lower, upper);

  BOOST_CHECK_EQUAL(lower.first, 0);
  BOOST_CHECK_EQUAL(lower.second, false);
  BOOST_CHECK_EQUAL(upper.first, 3);
  BOOST_CHECK_EQUAL(upper.second, false);
}

BOOST_AUTO_TEST_CASE( constraintToDBMTest )
{
  DBM guard;
  guard = DBM::zero(2);

  constraintsToDBM({TimedAutomaton::X(0) < 10}, guard);

  BOOST_CHECK_EQUAL(guard.value(1, 0).first, 10);
  BOOST_CHECK_EQUAL(guard.value(1, 0).second, false);
}

BOOST_AUTO_TEST_SUITE_END()
