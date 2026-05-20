#include <gtest/gtest.h>

#include "obxx/order.hpp"

using obxx::Decimal;
using obxx::Order;
using obxx::OrderBuilder;
using obxx::OrderIdGenerator;
using obxx::OrderRequest;
using obxx::OrderRequestBuilder;
using obxx::OrderSide;
using obxx::OrderStatus;
using obxx::OrderType;
using obxx::to_string;

TEST(OrderTest, ToStringCoversKnownStatuses)
{
  EXPECT_EQ(to_string(OrderStatus::Unfilled), "Unfilled");
  EXPECT_EQ(to_string(OrderStatus::PartiallyFilled), "PartiallyFilled");
  EXPECT_EQ(to_string(OrderStatus::Filled), "Filled");
  EXPECT_EQ(to_string(OrderStatus::Canceled), "Canceled");
  EXPECT_EQ(to_string(OrderStatus::Rejected), "Rejected");
  EXPECT_EQ(to_string(OrderStatus::Expired), "Expired");
}

TEST(OrderTest, ToStringUnknownStatusReturnsUnknown)
{
  EXPECT_EQ(to_string(static_cast<OrderStatus>(255)), "Unknown");
}

TEST(OrderTest, OrderIdGeneratorMonotonicallyIncrements)
{
  const auto id1 = OrderIdGenerator::next();
  const auto id2 = OrderIdGenerator::next();
  EXPECT_EQ(id2, id1 + 1);
}

TEST(OrderTest, OrderConstructedFromRequestCopiesRequestFields)
{
  OrderRequest request =
      OrderRequestBuilder(OrderType::Limit, OrderSide::Buy, 42).price(Decimal<2>::from_double(101.25)).build().value();

  Order order = *(OrderBuilder::build(request).second);

  EXPECT_EQ(order.status, OrderStatus::Unfilled);
  EXPECT_EQ(order.type, request.type);
  EXPECT_EQ(order.side, request.side);
  EXPECT_EQ(order.quantity, request.quantity);
  ASSERT_TRUE(order.price.has_value());
  EXPECT_EQ(order.price->to_string(), "101.25");
}

TEST(OrderTest, OrderRequestBuilderBuildsValidLimitOrder)
{
  auto request = OrderRequestBuilder(OrderType::Limit, OrderSide::Sell, 10).price(Decimal<2>::from_double(99.50)).build();

  ASSERT_TRUE(request.has_value());
  EXPECT_EQ(request->type, OrderType::Limit);
  EXPECT_EQ(request->side, OrderSide::Sell);
  EXPECT_EQ(request->quantity, 10);
  ASSERT_TRUE(request->price.has_value());
  EXPECT_EQ(request->price->to_string(), "99.50");
}

TEST(OrderTest, OrderRequestBuilderRejectsNegativeQuantity)
{
  auto request = OrderRequestBuilder(OrderType::Limit, OrderSide::Buy, -1).price(Decimal<2>::from_double(10.00)).build();
  EXPECT_FALSE(request.has_value());
}

TEST(OrderTest, OrderRequestBuilderRejectsMarketOrderWithPrice)
{
  auto request = OrderRequestBuilder(OrderType::Market, OrderSide::Buy, 5).price(Decimal<2>::from_double(10.00)).build();
  EXPECT_FALSE(request.has_value());
}

TEST(OrderTest, OrderRequestBuilderRejectsInvalidSide)
{
  auto request =
      OrderRequestBuilder(OrderType::Limit, static_cast<OrderSide>(255), 5).price(Decimal<2>::from_double(10.00)).build();
  EXPECT_FALSE(request.has_value());
}
