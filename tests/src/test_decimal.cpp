#include <gtest/gtest.h>

#include "obxx/decimal.hpp"

using obxx::Decimal;
using obxx::RoundingMode;

TEST(DecimalTest, DefaultConstructor)
{
  Decimal<2> d;
  EXPECT_EQ(d.raw(), 0);
  EXPECT_DOUBLE_EQ(d.to_double(), 0.0);
}

TEST(DecimalTest, Int64Constructor)
{
  Decimal<2> d(12345, false);  // represents 123.45
  EXPECT_EQ(d.to_string(), "123.45");
  EXPECT_DOUBLE_EQ(d.to_double(), 123.45);
}

TEST(DecimalTest, Int64ConstructorScaled)
{
  Decimal<2> d(12345, true);  // represents 12345.00
  EXPECT_EQ(d.to_string(), "12345.00");
  EXPECT_DOUBLE_EQ(d.to_double(), 12345.00);
}

TEST(DecimalTest, FromDoubleBasic)
{
  Decimal<2> d = Decimal<2>::from_double(1.23);
  EXPECT_EQ(d.to_string(), "1.23");
  EXPECT_NEAR(d.to_double(), 1.23, 1e-9);
}

TEST(DecimalTest, FromDoubleRounding)
{
  Decimal<2> d = Decimal<2>::from_double(1.235);  // should round
  EXPECT_EQ(d.to_string(), "1.24");
}

TEST(DecimalTest, NegativeValues)
{
  Decimal<2> d = Decimal<2>::from_double(-1.234);
  EXPECT_EQ(d.to_string(), "-1.23");
  EXPECT_LT(d.raw(), 0);
}

TEST(DecimalTest, ToStringPadding)
{
  Decimal<4> d = Decimal<4>::from_double(1.2);
  EXPECT_EQ(d.to_string(), "1.2000");
}

TEST(DecimalTest, Addition)
{
  Decimal<2> a = Decimal<2>::from_double(1.50);
  Decimal<2> b = Decimal<2>::from_double(2.25);

  Decimal<2> c = a + b;
  EXPECT_EQ(c.to_string(), "3.75");
}

TEST(DecimalTest, Subtraction)
{
  Decimal<2> a = Decimal<2>::from_double(5.00);
  Decimal<2> b = Decimal<2>::from_double(2.25);

  Decimal<2> c = a - b;
  EXPECT_EQ(c.to_string(), "2.75");
}

TEST(DecimalTest, Comparison)
{
  Decimal<2> a = Decimal<2>::from_double(1.00);
  Decimal<2> b = Decimal<2>::from_double(2.00);

  EXPECT_TRUE(a < b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(b >= a);
  EXPECT_FALSE(a == b);
}

TEST(DecimalTest, RoundDown)
{
  Decimal<3> d = Decimal<3>::from_double(1.999);

  auto r = d.round<2>(RoundingMode::Down);
  EXPECT_EQ(r.to_string(), "1.99");
}

TEST(DecimalTest, RoundUp)
{
  Decimal<3> d1 = Decimal<3>::from_double(1.991);

  auto r1 = d1.round<2>(RoundingMode::Up);
  EXPECT_EQ(r1.to_string(), "2.00");

  Decimal d2 = Decimal<3>::from_double(1.000);
  auto r2 = d2.round<2>(RoundingMode::Up);
  EXPECT_EQ(r2.to_string(), "1.00");
}

TEST(DecimalTest, RoundHalfUp)
{
  Decimal<3> d1 = Decimal<3>::from_double(1.995);

  auto r1 = d1.round<2>(RoundingMode::HalfUp);
  EXPECT_EQ(r1.to_string(), "2.00");

  Decimal<3> d2 = Decimal<3>::from_double(1.994);

  auto r2 = d2.round<2>(RoundingMode::HalfUp);
  EXPECT_EQ(r2.to_string(), "1.99");
}

TEST(DecimalTest, RoundHalfDown)
{
  Decimal<3> d = Decimal<3>::from_double(1.995);

  auto r = d.round<2>(RoundingMode::HalfDown);
  EXPECT_EQ(r.to_string(), "1.99");
}

TEST(DecimalTest, RoundHalfEven)
{
  Decimal<3> d1 = Decimal<3>::from_double(1.995);  // tie -> 2.00 (even)
  Decimal<3> d2 = Decimal<3>::from_double(2.005);  // tie -> 2.00 (even)

  EXPECT_EQ(d1.round<2>(RoundingMode::HalfEven).to_string(), "2.00");
  EXPECT_EQ(d2.round<2>(RoundingMode::HalfEven).to_string(), "2.00");

  // Test "default" value on switch statement
  Decimal<3> d3 = Decimal<3>::from_double(1.995);
  Decimal<3> d4 = Decimal<3>::from_double(2.005);

  EXPECT_EQ(d3.round<2>(RoundingMode::Count).to_string(), "2.00");
  EXPECT_EQ(d4.round<2>(RoundingMode::Count).to_string(), "2.00");
}

TEST(DecimalTest, RoundHalfOdd)
{
  Decimal<3> d1 = Decimal<3>::from_double(1.995);
  auto r1 = d1.round<2>(RoundingMode::HalfOdd);
  EXPECT_EQ(r1.to_string(), "1.99");

  Decimal<3> d2 = Decimal<3>::from_double(2.005);
  auto r2 = d2.round<2>(RoundingMode::HalfOdd);
  EXPECT_EQ(r2.to_string(), "2.01");

  Decimal<3> d3 = Decimal<3>::from_double(1.999);
  auto r3 = d3.round<2>(RoundingMode::HalfOdd);
  EXPECT_EQ(r3.to_string(), "2.00");
}

TEST(DecimalTest, RoundNoPrecisionIncrease)
{
  Decimal<2> d = Decimal<2>::from_double(1.23);

  auto r = d.round<4>();
  EXPECT_EQ(r.to_string(), "1.2300");
}

TEST(DecimalTest, ZeroHandling)
{
  Decimal<3> d = Decimal<3>::from_double(0.0);
  EXPECT_EQ(d.to_string(), "0.000");
}

TEST(DecimalTest, NegativeZeroHandling)
{
  Decimal<3> d = Decimal<3>::from_double(-0.0);
  EXPECT_EQ(d.to_string(), "0.000");  // should treat -0.0 as 0.000
}

TEST(DecimalTest, Int64Bounds)
{
  Decimal<2> d1(std::numeric_limits<int64_t>::max());
  Decimal<2> d2(std::numeric_limits<int64_t>::min());

  EXPECT_EQ(d1.to_string(), "92233720368547758.07");   // Max int64_t scaled by 10^2
  EXPECT_EQ(d2.to_string(), "-92233720368547758.08");  // Min int64_t scaled by 10^2
}

TEST(DecimalTest, FromDoubleBounds)
{
  Decimal<2> d1 = Decimal<2>::from_double(static_cast<double>(std::numeric_limits<int64_t>::max()));
  Decimal<2> d2 = Decimal<2>::from_double(static_cast<double>(std::numeric_limits<int64_t>::lowest()));

  EXPECT_EQ(d1.to_string(), "0.00");  // Should return 0 on overflow
  EXPECT_EQ(d2.to_string(), "0.00");  // Should return 0 on underflow
}
