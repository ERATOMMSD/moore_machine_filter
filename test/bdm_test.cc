#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/bdm.hh"

BOOST_AUTO_TEST_SUITE(BDMTest)

BOOST_AUTO_TEST_CASE( findDurationTest )
{
  BDM orig, guard;
  orig = BDM::zero(3);
  guard = BDM::zero(3);

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

BOOST_AUTO_TEST_SUITE_END()
