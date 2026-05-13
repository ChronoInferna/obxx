#pragma once

#include <atomic>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

#include "decimal.hpp"

namespace obxx
{

  enum class OrderStatus : uint8_t
  {
    Unfilled,
    PartiallyFilled,
    Filled,
    Canceled,
    Rejected,
    Expired
  };

  using OrderId = uint64_t;
  class OrderIdGenerator
  {
   private:
    inline static std::atomic<OrderId> current_id_ = 0;

   public:
    static OrderId next();
  };

  using OrderQuantity = int64_t;

  enum class OrderSide : uint8_t
  {
    Buy,
    Sell
  };

  enum class OrderType : uint8_t
  {
    Market,
    Limit,
    Stop,
    StopLimit,
    Count
  };

  struct OrderRequest
  {
    OrderSide side;
    OrderQuantity quantity;
    OrderType type;

    // Optional fields
    std::optional<Decimal<2>> price;
  };

  struct Order
  {
    OrderStatus status;

    OrderSide side;
    OrderQuantity quantity;
    OrderType type;

    // Optional fields
    std::optional<Decimal<2>> price;

    Order(const OrderRequest& request);
  };

  class OrderRequestBuilder
  {
   private:
    std::unique_ptr<OrderRequest> request_;

   public:
    OrderRequestBuilder(OrderSide side, OrderQuantity quantity, OrderType type);
    OrderRequest& price(Decimal<2> price);
    std::expected<OrderRequest, std::string> build();
  };

}  // namespace obxx
