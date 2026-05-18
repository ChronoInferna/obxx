#include <gtest/gtest.h>

#include "obxx/order_book.hpp"

using obxx::Decimal;
using obxx::OrderBook;
using obxx::OrderRequest;
using obxx::OrderSide;
using obxx::OrderStatus;
using obxx::OrderType;

TEST(OrderBookTest, QueryBestBidAndAskReturnErrorWhenBookIsEmpty)
{
  OrderBook book;

  EXPECT_FALSE(book.query_best_bid().has_value());
  EXPECT_FALSE(book.query_best_ask().has_value());
}

TEST(OrderBookTest, SubmitLimitBuyAddsOrderToBookAndQueriesSucceed)
{
  OrderBook book;

  OrderRequest request{
    .type = OrderType::Limit,
    .side = OrderSide::Buy,
    .quantity = 12,
    .price = Decimal<2>::from_double(100.10),
  };

  const auto id = book.submit_order_request(request);
  const auto order = book.query_order_id(id);
  const auto best_bid = book.query_best_bid();
  const auto volume = book.query_volume_at_price(Decimal<2>::from_double(100.10));

  ASSERT_TRUE(order.has_value());
  EXPECT_EQ(order->status, OrderStatus::Unfilled);
  EXPECT_EQ(order->quantity, 12);

  ASSERT_TRUE(best_bid.has_value());
  EXPECT_EQ(best_bid->to_string(), "100.10");

  ASSERT_TRUE(volume.has_value());
  EXPECT_EQ(*volume, 12);
}

TEST(OrderBookTest, CancelExistingBuyOrderMarksCanceledAndRemovesVolume)
{
  OrderBook book;

  OrderRequest request{
    .type = OrderType::Limit,
    .side = OrderSide::Buy,
    .quantity = 7,
    .price = Decimal<2>::from_double(101.00),
  };

  const auto id = book.submit_order_request(request);
  ASSERT_TRUE(book.cancel_order(id));

  const auto order = book.query_order_id(id);
  ASSERT_TRUE(order.has_value());
  EXPECT_EQ(order->status, OrderStatus::Canceled);

  const auto volume = book.query_volume_at_price(Decimal<2>::from_double(101.00));
  ASSERT_TRUE(volume.has_value());
  EXPECT_EQ(*volume, 0);
}

TEST(OrderBookTest, CancelUnknownOrderReturnsFalse)
{
  OrderBook book;
  EXPECT_FALSE(book.cancel_order(999999));
}

TEST(OrderBookTest, QueryMissingOrderAndPriceReturnErrors)
{
  OrderBook book;
  EXPECT_FALSE(book.query_order_id(12345).has_value());
  EXPECT_FALSE(book.query_volume_at_price(Decimal<2>::from_double(88.88)).has_value());
}

TEST(OrderBookTest, MarketBuyOnEmptyBookEndsCanceled)
{
  OrderBook book;
  OrderRequest request{
    .type = OrderType::Market,
    .side = OrderSide::Buy,
    .quantity = 3,
    .price = std::nullopt,
  };

  const auto id = book.submit_order_request(request);
  const auto order = book.query_order_id(id);

  ASSERT_TRUE(order.has_value());
  EXPECT_EQ(order->status, OrderStatus::Canceled);
}
