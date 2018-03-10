#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include "../src/bit_buffer.hh"

BOOST_AUTO_TEST_SUITE(BitBufferTest)

BOOST_AUTO_TEST_CASE( example1 )
{
  BitBuffer<5> buffer;

  BOOST_CHECK_EQUAL(buffer.getAndEnable(3), false);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b111);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), false);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b1110);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), false);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b11100);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), true);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b11000);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), true);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b10000);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), true);
  BOOST_CHECK_EQUAL(buffer.bitBuffer.to_ulong(), 0b00000);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), false);
  BOOST_CHECK_EQUAL(buffer.getAndEnable(0), false);
}

BOOST_AUTO_TEST_SUITE_END()
