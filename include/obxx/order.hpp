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

  std::string_view to_string(OrderStatus status);

  using OrderId = uint64_t;
  class OrderIdGenerator
  {
   private:
    inline static std::atomic<OrderId> current_id_ = 0;

   public:
    static OrderId next();
  };

  enum class OrderType : uint8_t
  {
    Market,
    Limit,
    Stop,
    StopLimit,
    Count
  };

  enum class OrderSide : uint8_t
  {
    Buy,
    Sell
  };

  using OrderQuantity = int64_t;

  struct OrderRequest
  {
    OrderType type;
    OrderSide side;
    OrderQuantity quantity;

    // Optional fields
    std::optional<Decimal<2>> price;
  };

  struct Order
  {
    OrderStatus status;

    OrderType type;
    OrderSide side;
    OrderQuantity quantity;

    // Optional fields
    std::optional<Decimal<2>> price;

    Order(const OrderRequest& request);
  };

  class OrderRequestBuilder
  {
   private:
    std::unique_ptr<OrderRequest> request_;

   public:
    OrderRequestBuilder(OrderType type, OrderSide side, OrderQuantity quantity);
    OrderRequestBuilder& price(Decimal<2> price);
    std::expected<OrderRequest, std::string> build();
  };

}  // namespace obxx
